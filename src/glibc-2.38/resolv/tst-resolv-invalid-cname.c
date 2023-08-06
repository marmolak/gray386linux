/* Test handling of CNAMEs with non-host domain names (bug 12154).
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

#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <stdlib.h>
#include <string.h>
#include <support/check.h>
#include <support/check_nss.h>
#include <support/resolv_test.h>
#include <support/support.h>
#include <support/xmemstream.h>

/* Query strings describe the CNAME chain in the response.  They have
   the format "bitsBITS.countCOUNT.example.", where BITS and COUNT are
   replaced by unsigned decimal numbers.  COUNT is the number of CNAME
   records in the response.  BITS has two bits for each CNAME record,
   describing a special prefix that is added to that CNAME.

   0: No special leading label.
   1: Starting with "*.".
   2: Starting with "-x.".
   3: Starting with "star.*.".

   The first CNAME in the response using the two least significant
   bits.

   For PTR queries, the QNAME format is different, it is either
   COUNT.BITS.168.192.in-addr.arpa. (with BITS and COUNT still
   decimal), or:

COUNT.BITS0.BITS1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.8.b.d.0.1.0.0.2.ip6.arpa.

   where BITS and COUNT are hexadecimal.  */

static void
response (const struct resolv_response_context *ctx,
          struct resolv_response_builder *b,
          const char *qname, uint16_t qclass, uint16_t qtype)
{
  TEST_COMPARE (qclass, C_IN);

  /* The only other query type besides A is PTR.  */
  if (qtype != T_A && qtype != T_AAAA)
    TEST_COMPARE (qtype, T_PTR);

  unsigned int bits, bits1, count;
  char *tail = NULL;
  if (sscanf (qname, "bits%u.count%u.%ms", &bits, &count, &tail) == 3)
    TEST_COMPARE_STRING (tail, "example");
  else if (strstr (qname, "in-addr.arpa") != NULL
           && sscanf (qname, "%u.%u.%ms", &bits, &count, &tail) == 3)
    TEST_COMPARE_STRING (tail, "168.192.in-addr.arpa");
  else if (sscanf (qname, "%x.%x.%x.%ms", &bits, &bits1, &count, &tail) == 4)
    {
      TEST_COMPARE_STRING (tail, "\
0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.8.b.d.0.1.0.0.2.ip6.arpa");
      bits |= bits1 << 4;
    }
  else
    FAIL_EXIT1 ("invalid QNAME: %s\n", qname);
  free (tail);

  struct resolv_response_flags flags = {};
  resolv_response_init (b, flags);
  resolv_response_add_question (b, qname, qclass, qtype);
  resolv_response_section (b, ns_s_an);

  /* Provide the requested number of CNAME records.  */
  char *previous_name = (char *) qname;
  unsigned int original_bits = bits;
  for (int unique = 0; unique < count; ++unique)
    {
      resolv_response_open_record (b, previous_name, qclass, T_CNAME, 60);

      static const char bits_to_prefix[4][8] = { "", "*.", "-x.", "star.*." };
      char *new_name = xasprintf ("%sunique%d.example",
                                  bits_to_prefix[bits & 3], unique);
      bits >>= 2;
      resolv_response_add_name (b, new_name);
      resolv_response_close_record (b);

      if (previous_name != qname)
        free (previous_name);
      previous_name = new_name;
    }

  /* Actual answer record.  */
  resolv_response_open_record (b, previous_name, qclass, qtype, 60);
  switch (qtype)
    {
    case T_A:
      {
        char ipv4[4] = {192, 168, count, original_bits};
        resolv_response_add_data (b, &ipv4, sizeof (ipv4));
      }
      break;
    case T_AAAA:
      {
        char ipv6[16] =
          {
            0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            count, original_bits
          };
        resolv_response_add_data (b, &ipv6, sizeof (ipv6));
      }
      break;

    case T_PTR:
      {
        char *name = xasprintf ("bits%u.count%u.example",
                                original_bits, count);
        resolv_response_add_name (b, name);
        free (name);
      }
      break;
    }
  resolv_response_close_record (b);

  if (previous_name != qname)
    free (previous_name);
}

/* Controls which name resolution function is invoked.  */
enum test_mode
  {
    byname,                     /* gethostbyname.  */
    byname2,                    /* gethostbyname2.  */
    gai,                        /* getaddrinfo without AI_CANONNAME.  */
    gai_canon,                  /* getaddrinfo with AI_CANONNAME.  */

    test_mode_num               /* Number of enum values.  */
  };

static const char *
test_mode_to_string (enum test_mode mode)
{
  switch (mode)
    {
    case byname:
      return "byname";
    case byname2:
      return "byname2";
    case gai:
      return "gai";
    case gai_canon:
      return "gai_canon";
    case test_mode_num:
      break;                    /* Report error below.  */
    }
  FAIL_EXIT1 ("invalid test_mode: %d", mode);
}

/* Append the name and aliases to OUT.  */
static void
append_names (FILE *out, const char *qname, int bits, int count,
              enum test_mode mode)
{
  /* Largest valid index which has a corresponding zero in bits
     (meaning a syntactically valid CNAME).  */
  int last_valid_cname = -1;

  for (int i = 0; i < count; ++i)
    if ((bits & (3 << (i * 2))) == 0)
      last_valid_cname = i;

  if (mode != gai)
    {
      const char *label;
      if (mode == gai_canon)
        label = "canonname";
      else
        label = "name";
      if (last_valid_cname >= 0)
        fprintf (out, "%s: unique%d.example\n", label, last_valid_cname);
      else
        fprintf (out, "%s: %s\n", label, qname);
    }

  if (mode == byname || mode == byname2)
    {
      if (last_valid_cname >= 0)
        fprintf (out, "alias: %s\n", qname);
      for (int i = 0; i < count; ++i)
        {
          if ((bits & (3 << (i * 2))) == 0 && i != last_valid_cname)
            fprintf (out, "alias: unique%d.example\n", i);
        }
    }
}

/* Append the address information to OUT.  */
static void
append_addresses (FILE *out, int af, int bits, int count, enum test_mode mode)
{
  int last = count * 256 + bits;
  if (mode == gai || mode == gai_canon)
    {
      if (af == AF_INET || af == AF_UNSPEC)
        fprintf (out, "address: STREAM/TCP 192.168.%d.%d 80\n", count, bits);
      if (af == AF_INET6 || af == AF_UNSPEC)
        {
          if (last == 0)
            fprintf (out, "address: STREAM/TCP 2001:db8:: 80\n");
          else
            fprintf (out, "address: STREAM/TCP 2001:db8::%x 80\n", last);
        }
    }
  else
    {
      TEST_VERIFY (af != AF_UNSPEC);
      if (af == AF_INET)
        fprintf (out, "address: 192.168.%d.%d\n", count, bits);
      if (af == AF_INET6)
        {
          if (last == 0)
            fprintf (out, "address: 2001:db8::\n");
          else
            fprintf (out, "address: 2001:db8::%x\n", last);
        }
    }
}

/* Perform one test using a forward lookup.  */
static void
check_forward (int af, int bits, int count, enum test_mode mode)
{
  char *qname = xasprintf ("bits%d.count%d.example", bits, count);
  char *label = xasprintf ("af=%d bits=%d count=%d mode=%s qname=%s",
                           af, bits, count, test_mode_to_string (mode), qname);

  struct xmemstream expected;
  xopen_memstream (&expected);
  if (mode == gai_canon)
    fprintf (expected.out, "flags: AI_CANONNAME\n");
  append_names (expected.out, qname, bits, count, mode);
  append_addresses (expected.out, af, bits, count, mode);
  xfclose_memstream (&expected);

  if (mode == gai || mode == gai_canon)
    {
      struct addrinfo *ai;
      struct addrinfo hints =
        {
          .ai_family = af,
          .ai_socktype = SOCK_STREAM,
        };
      if (mode == gai_canon)
        hints.ai_flags |= AI_CANONNAME;
      int ret = getaddrinfo (qname, "80", &hints, &ai);
      check_addrinfo (label, ai, ret, expected.buffer);
      if (ret == 0)
        freeaddrinfo (ai);
    }
  else
    {
      struct hostent *e;
      if (mode == gai)
        {
          TEST_COMPARE (af, AF_INET);
          e = gethostbyname (qname);
        }
      else
        {
          if (af != AF_INET)
            TEST_COMPARE (af, AF_INET6);
          e = gethostbyname2 (qname, af);
        }
      check_hostent (label, e, expected.buffer);
    }

  free (expected.buffer);
  free (label);
  free (qname);
}

/* Perform one check using a reverse lookup.  */

static void
check_reverse (int af, int bits, int count)
{
  TEST_VERIFY (af == AF_INET || af == AF_INET6);

  char *label = xasprintf ("af=%d bits=%d count=%d", af, bits, count);
  char *fqdn = xasprintf ("bits%d.count%d.example", bits, count);

  struct xmemstream expected;
  xopen_memstream (&expected);
  fprintf (expected.out, "name: %s\n", fqdn);
  append_addresses (expected.out, af, bits, count, byname);
  xfclose_memstream (&expected);

  char addr[16] = { 0 };
  socklen_t addrlen;
  if (af == AF_INET)
    {
      addr[0] = 192;
      addr[1] = 168;
      addr[2] = count;
      addr[3] = bits;
      addrlen = 4;
    }
  else
    {
      addr[0] = 0x20;
      addr[1] = 0x01;
      addr[2] = 0x0d;
      addr[3] = 0xb8;
      addr[14] = count;
      addr[15] = bits;
      addrlen = 16;
    }

  struct hostent *e = gethostbyaddr (addr, addrlen, af);
  check_hostent (label, e, expected.buffer);

  /* getnameinfo check is different.  There is no generic check_*
     function for it.  */
  {
    struct sockaddr_in sin = { };
    struct sockaddr_in6 sin6 = { };
    void *sa;
    socklen_t salen;
    if (af == AF_INET)
      {
        sin.sin_family = AF_INET;
        memcpy (&sin.sin_addr, addr, addrlen);
        sin.sin_port = htons (80);
        sa = &sin;
        salen = sizeof (sin);
      }
    else
      {
        sin6.sin6_family = AF_INET6;
        memcpy (&sin6.sin6_addr, addr, addrlen);
        sin6.sin6_port = htons (80);
        sa = &sin6;
        salen = sizeof (sin6);
      }

    char host[64];
    char service[64];
    int ret = getnameinfo (sa, salen, host,
                           sizeof (host), service, sizeof (service),
                           NI_NAMEREQD | NI_NUMERICSERV);
    TEST_COMPARE (ret, 0);
    TEST_COMPARE_STRING (host, fqdn);
    TEST_COMPARE_STRING (service, "80");
  }

  free (expected.buffer);
  free (fqdn);
  free (label);
}

static int
do_test (void)
{
  struct resolv_test *obj = resolv_test_start
    ((struct resolv_redirect_config)
     {
       .response_callback = response
     });

  for (int count = 0; count <= 3; ++count)
    for (int bits = 0; bits <= 1 << (count * 2); ++bits)
      {
        if (count > 0 && bits == count)
          /* The last bits value is only checked if count == 0.  */
          continue;

        for (enum test_mode mode = 0; mode < test_mode_num; ++mode)
          {
            check_forward (AF_INET, bits, count, mode);
            if (mode != byname)
              check_forward (AF_INET6, bits, count, mode);
            if (mode == gai || mode == gai_canon)
              check_forward (AF_UNSPEC, bits, count, mode);
          }

        check_reverse (AF_INET, bits, count);
        check_reverse (AF_INET6, bits, count);
      }

  resolv_test_end (obj);

  return 0;
}

#include <support/test-driver.c>
