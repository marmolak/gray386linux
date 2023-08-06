/* Test reverse DNS lookup.
   Copyright (C) 2022-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <support/check.h>
#include <support/check_nss.h>
#include <support/next_to_fault.h>
#include <support/resolv_test.h>
#include <support/support.h>

#include "tst-resolv-maybe_insert_sig.h"

/* QNAME format:

   ADDRESSES.CNAMES...(lots of 0s)...8.b.d.0.1.0.0.2.ip6.arpa.
   CNAMES|ADDRESSES.2.0.192.in-addr-arpa.

   For the IPv4 reverse lookup, the address count is in the lower
   bits.

   CNAMES is the length of the CNAME chain, ADDRESSES is the number of
   addresses in the response.  The special value 15 means that there
   are no addresses, and the RCODE is NXDOMAIN.  */
static void
response (const struct resolv_response_context *ctx,
          struct resolv_response_builder *b,
          const char *qname, uint16_t qclass, uint16_t qtype)
{
  TEST_COMPARE (qclass, C_IN);
  TEST_COMPARE (qtype, T_PTR);

  unsigned int addresses, cnames, bits;
  char *tail;
  if (strstr (qname, "ip6.arpa") != NULL
      && sscanf (qname, "%x.%x.%ms", &addresses, &cnames, &tail) == 3)
    TEST_COMPARE_STRING (tail, "\
0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.8.b.d.0.1.0.0.2.ip6.arpa");
  else if (sscanf (qname, "%u.%ms", &bits, &tail) == 2)
    {
      TEST_COMPARE_STRING (tail, "2.0.192.in-addr.arpa");
      addresses = bits & 0x0f;
      cnames = bits >> 4;
    }
  else
    FAIL_EXIT1 ("invalid QNAME: %s", qname);
  free (tail);

  int rcode;
  if (addresses == 15)
    {
      /* Special case: Use no addresses with NXDOMAIN response.  */
      rcode = ns_r_nxdomain;
      addresses = 0;
    }
  else
    rcode = 0;

  struct resolv_response_flags flags = { .rcode = rcode };
  resolv_response_init (b, flags);
  resolv_response_add_question (b, qname, qclass, qtype);
  resolv_response_section (b, ns_s_an);
  maybe_insert_sig (b, qname);

  /* Provide the requested number of CNAME records.  */
  char *previous_name = (char *) qname;
  for (int unique = 0; unique < cnames; ++unique)
    {
      resolv_response_open_record (b, previous_name, qclass, T_CNAME, 60);
      char *new_name = xasprintf ("%d.alias.example", unique);
      resolv_response_add_name (b, new_name);
      resolv_response_close_record (b);

      maybe_insert_sig (b, qname);

      if (previous_name != qname)
        free (previous_name);
      previous_name = new_name;
    }

  for (int unique = 0; unique < addresses; ++unique)
    {
      resolv_response_open_record (b, previous_name, qclass, T_PTR, 60);
      char *ptr = xasprintf ("unique-%d.cnames-%u.addresses-%u.example",
                             unique, cnames, addresses);
      resolv_response_add_name (b, ptr);
      free (ptr);
      resolv_response_close_record (b);
    }

  if (previous_name != qname)
    free (previous_name);
}

/* Used to check that gethostbyaddr_r does not write past the buffer
   end.  */
static struct support_next_to_fault ntf;

/* Perform a gethostbyaddr call and check the result.  */
static void
check_gethostbyaddr (const char *address, const char *expected)
{
  unsigned char bytes[16];
  unsigned int byteslen;
  int family;
  if (strchr (address, ':') != NULL)
    {
      family = AF_INET6;
      byteslen = 16;
    }
  else
    {
      family = AF_INET;
      byteslen = 4;
    }
  TEST_COMPARE (inet_pton (family, address, bytes), 1);

  struct hostent *e = gethostbyaddr (bytes, byteslen, family);
  check_hostent (address, e, expected);

  if (e == NULL)
    return;

  /* Try gethostbyaddr_r with increasing sizes until success.  First
     compute a reasonable minimum buffer size, to avoid many pointless
     attempts.  */
  size_t minimum_size = strlen (e->h_name);
  for (int i = 0; e->h_addr_list[i] != NULL; ++i)
    minimum_size += e->h_length + sizeof (char *);
  for (int i = 0; e->h_aliases[i] != NULL; ++i)
    minimum_size += strlen (e->h_aliases[i]) + 1 + sizeof (char *);

  /* Gradually increase the size until success.  */
  for (size_t size = minimum_size; size < ntf.length; ++size)
    {
      struct hostent result;
      int herrno;
      int ret = gethostbyaddr_r (bytes, byteslen, family, &result,
                                 ntf.buffer + ntf.length - size, size,
                                 &e, &herrno);
      if (ret == ERANGE)
        /* Retry with larger size.  */
        TEST_COMPARE (herrno, NETDB_INTERNAL);
      else if (ret == 0)
        {
         TEST_VERIFY (size > minimum_size);
         check_hostent (address, e, expected);
         return;
        }
      else
        FAIL_EXIT1 ("Unexpected gethostbyaddr_r failure: %d", ret);
    }

  FAIL_EXIT1 ("gethostbyaddr_r always failed for: %s", address);
}

/* Perform a getnameinfo call and check the result.  */
static void
check_getnameinfo (const char *address, const char *expected)
{
  struct sockaddr_in sin = { };
  struct sockaddr_in6 sin6 = { };
  void *sa;
  socklen_t salen;
  if (strchr (address, ':') != NULL)
    {
      sin6.sin6_family = AF_INET6;
      TEST_COMPARE (inet_pton (AF_INET6, address, &sin6.sin6_addr), 1);
      sin6.sin6_port = htons (80);
      sa = &sin6;
      salen = sizeof (sin6);
    }
  else
    {
      sin.sin_family = AF_INET;
      TEST_COMPARE (inet_pton (AF_INET, address, &sin.sin_addr), 1);
      sin.sin_port = htons (80);
      sa = &sin;
      salen = sizeof (sin);
    }

  char host[64];
  char service[64];
  int ret = getnameinfo (sa, salen, host,
                         sizeof (host), service, sizeof (service),
                         NI_NAMEREQD | NI_NUMERICSERV);
  switch (ret)
    {
    case 0:
      TEST_COMPARE_STRING (host, expected);
      TEST_COMPARE_STRING (service, "80");
      break;
    case EAI_SYSTEM:
      TEST_COMPARE_STRING (strerror (errno), expected);
      break;
    default:
      TEST_COMPARE_STRING (gai_strerror (ret), expected);
    }
}

static int
do_test (void)
{
  /* Some reasonably upper bound for the maximum response size.  */
  ntf = support_next_to_fault_allocate (4096);

  struct resolv_test *obj = resolv_test_start
    ((struct resolv_redirect_config)
     {
       .response_callback = response
     });

  for (int do_insert_sig = 0; do_insert_sig < 2; ++do_insert_sig)
    {
      insert_sig = do_insert_sig;

      /* No PTR record, RCODE=0.  */
      check_gethostbyaddr ("192.0.2.0", "error: NO_RECOVERY\n");
      check_getnameinfo ("192.0.2.0", "Name or service not known");
      check_gethostbyaddr ("192.0.2.16", "error: NO_RECOVERY\n");
      check_getnameinfo ("192.0.2.16", "Name or service not known");
      check_gethostbyaddr ("192.0.2.32", "error: NO_RECOVERY\n");
      check_getnameinfo ("192.0.2.32", "Name or service not known");
      check_gethostbyaddr ("2001:db8::", "error: NO_RECOVERY\n");
      check_getnameinfo ("2001:db8::", "Name or service not known");
      check_gethostbyaddr ("2001:db8::10", "error: NO_RECOVERY\n");
      check_getnameinfo ("2001:db8::10", "Name or service not known");
      check_gethostbyaddr ("2001:db8::20", "error: NO_RECOVERY\n");
      check_getnameinfo ("2001:db8::20", "Name or service not known");

      /* No PTR record, NXDOMAIN.  */
      check_gethostbyaddr ("192.0.2.15", "error: HOST_NOT_FOUND\n");
      check_getnameinfo ("192.0.2.15", "Name or service not known");
      check_gethostbyaddr ("192.0.2.31", "error: HOST_NOT_FOUND\n");
      check_getnameinfo ("192.0.2.31", "Name or service not known");
      check_gethostbyaddr ("192.0.2.47", "error: HOST_NOT_FOUND\n");
      check_getnameinfo ("192.0.2.47", "Name or service not known");
      check_gethostbyaddr ("2001:db8::f", "error: HOST_NOT_FOUND\n");
      check_getnameinfo ("2001:db8::f", "Name or service not known");
      check_gethostbyaddr ("2001:db8::1f", "error: HOST_NOT_FOUND\n");
      check_getnameinfo ("2001:db8::1f", "Name or service not known");
      check_gethostbyaddr ("2001:db8::2f", "error: HOST_NOT_FOUND\n");
      check_getnameinfo ("2001:db8::2f", "Name or service not known");

      /* Actual response data.  Only the first PTR record is returned.  */
      check_gethostbyaddr ("192.0.2.1",
                           "name: unique-0.cnames-0.addresses-1.example\n"
                           "address: 192.0.2.1\n");
      check_getnameinfo ("192.0.2.1",
                         "unique-0.cnames-0.addresses-1.example");
      check_gethostbyaddr ("192.0.2.17",
                           "name: unique-0.cnames-1.addresses-1.example\n"
                           "address: 192.0.2.17\n");
      check_getnameinfo ("192.0.2.17",
                         "unique-0.cnames-1.addresses-1.example");
      check_gethostbyaddr ("192.0.2.18",
                           "name: unique-0.cnames-1.addresses-2.example\n"
                           "address: 192.0.2.18\n");
      check_getnameinfo ("192.0.2.18",
                         "unique-0.cnames-1.addresses-2.example");
      check_gethostbyaddr ("192.0.2.33",
                           "name: unique-0.cnames-2.addresses-1.example\n"
                           "address: 192.0.2.33\n");
      check_getnameinfo ("192.0.2.33",
                         "unique-0.cnames-2.addresses-1.example");
      check_gethostbyaddr ("192.0.2.34",
                           "name: unique-0.cnames-2.addresses-2.example\n"
                           "address: 192.0.2.34\n");
      check_getnameinfo ("192.0.2.34",
                         "unique-0.cnames-2.addresses-2.example");

      /* Same for IPv6 addresses.  */
      check_gethostbyaddr ("2001:db8::1",
                           "name: unique-0.cnames-0.addresses-1.example\n"
                           "address: 2001:db8::1\n");
      check_getnameinfo ("2001:db8::1",
                         "unique-0.cnames-0.addresses-1.example");
      check_gethostbyaddr ("2001:db8::11",
                           "name: unique-0.cnames-1.addresses-1.example\n"
                           "address: 2001:db8::11\n");
      check_getnameinfo ("2001:db8::11",
                         "unique-0.cnames-1.addresses-1.example");
      check_gethostbyaddr ("2001:db8::12",
                           "name: unique-0.cnames-1.addresses-2.example\n"
                           "address: 2001:db8::12\n");
      check_getnameinfo ("2001:db8::12",
                         "unique-0.cnames-1.addresses-2.example");
      check_gethostbyaddr ("2001:db8::21",
                           "name: unique-0.cnames-2.addresses-1.example\n"
                           "address: 2001:db8::21\n");
      check_getnameinfo ("2001:db8::21",
                         "unique-0.cnames-2.addresses-1.example");
      check_gethostbyaddr ("2001:db8::22",
                           "name: unique-0.cnames-2.addresses-2.example\n"
                           "address: 2001:db8::22\n");
      check_getnameinfo ("2001:db8::22",
                         "unique-0.cnames-2.addresses-2.example");
    }

  resolv_test_end (obj);

  support_next_to_fault_free (&ntf);
  return 0;
}

#include <support/test-driver.c>
