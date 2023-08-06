/* Test alias handling (mainly for gethostbyname).
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

#include <array_length.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <support/check.h>
#include <support/check_nss.h>
#include <support/resolv_test.h>
#include <support/support.h>

#include "tst-resolv-maybe_insert_sig.h"

/* QNAME format:

   aADDRESSES-cCNAMES.example.net

   CNAMES is the length of the CNAME chain, ADDRESSES is the number of
   addresses in the response.  The special value 255 means that there
   are no addresses, and the RCODE is NXDOMAIN.  */
static void
response (const struct resolv_response_context *ctx,
          struct resolv_response_builder *b,
          const char *qname, uint16_t qclass, uint16_t qtype)
{
  TEST_COMPARE (qclass, C_IN);
  if (qtype != T_A)
    TEST_COMPARE (qtype, T_AAAA);

  unsigned int addresses, cnames;
  char *tail;
  if (sscanf (qname, "a%u-c%u%ms", &addresses, &cnames, &tail) == 3)
    {
      if (strcmp (tail, ".example.com") == 0
          || strcmp (tail, ".example.net.example.net") == 0
          || strcmp (tail, ".example.net.example.com") == 0)
        /* These only happen after NXDOMAIN.  */
        TEST_VERIFY (addresses == 255);
      else if (strcmp (tail, ".example.net") != 0)
        FAIL_EXIT1 ("invalid QNAME: %s", qname);
    }
  free (tail);

  int rcode;
  if (addresses == 255)
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
      resolv_response_open_record (b, previous_name, qclass, qtype, 60);

      if (qtype == T_A)
        {
          char ipv4[4] = {192, 0, 2, 1 + unique};
          resolv_response_add_data (b, &ipv4, sizeof (ipv4));
        }
      else if (qtype == T_AAAA)
        {
          char ipv6[16] =
            {
              0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              1 + unique
            };
          resolv_response_add_data (b, &ipv6, sizeof (ipv6));
        }
      resolv_response_close_record (b);
    }

  if (previous_name != qname)
    free (previous_name);
}

static char *
make_qname (bool do_search, int cnames, int addresses)
{
  return xasprintf ("a%d-c%d%s",
                    addresses, cnames, do_search ? "" : ".example.net");
}

static void
check_cnames_failure (int af, bool do_search, int cnames, int addresses)
{
  char *qname = make_qname (do_search, cnames, addresses);

  struct hostent *e;
  if (af == AF_UNSPEC)
    e = gethostbyname (qname);
  else
    e = gethostbyname2 (qname, af);

  if (addresses == 0)
    check_hostent (qname, e, "error: NO_RECOVERY\n");
  else
    check_hostent (qname, e, "error: HOST_NOT_FOUND\n");

  free (qname);
}

static void
check (int af, bool do_search, int cnames, int addresses)
{
  char *qname = make_qname (do_search, cnames, addresses);
  char *fqdn = make_qname (false, cnames, addresses);

  struct hostent *e;
  if (af == AF_UNSPEC)
    e = gethostbyname (qname);
  else
    e = gethostbyname2 (qname, af);
  if (e == NULL)
    FAIL_EXIT1 ("unexpected failure for %d, %d, %d", af, cnames, addresses);

  if (af == AF_UNSPEC || af == AF_INET)
    {
      TEST_COMPARE (e->h_addrtype, AF_INET);
      TEST_COMPARE (e->h_length, 4);
    }
  else
    {
      TEST_COMPARE (e->h_addrtype, AF_INET6);
      TEST_COMPARE (e->h_length, 16);
    }

  for (int i = 0; i < addresses; ++i)
    {
      char ipv4[4] = {192, 0, 2, 1 + i};
      char ipv6[16] =
        { 0x20, 0x01, 0xd, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 + i };
      char *expected = e->h_addrtype == AF_INET ? ipv4 : ipv6;
      TEST_COMPARE_BLOB (e->h_addr_list[i], e->h_length,
                         expected, e->h_length);
    }
  TEST_VERIFY (e->h_addr_list[addresses] == NULL);


  if (cnames == 0)
    {
      /* QNAME is fully qualified.  */
      TEST_COMPARE_STRING (e->h_name, fqdn);
      TEST_VERIFY (e->h_aliases[0] == NULL);
    }
  else
   {
     /* Fully-qualified QNAME is demoted to an aliases.  */
     TEST_COMPARE_STRING (e->h_aliases[0], fqdn);

     for (int i = 1; i <= cnames; ++i)
       {
         char *expected = xasprintf ("%d.alias.example", i - 1);
         if (i == cnames)
           TEST_COMPARE_STRING (e->h_name, expected);
         else
           TEST_COMPARE_STRING (e->h_aliases[i], expected);
         free (expected);
       }
     TEST_VERIFY (e->h_aliases[cnames] == NULL);
   }

  free (fqdn);
  free (qname);
}

static int
do_test (void)
{
  struct resolv_test *obj = resolv_test_start
    ((struct resolv_redirect_config)
     {
       .response_callback = response,
       .search = { "example.net", "example.com" },
     });

  static const int families[] = { AF_UNSPEC, AF_INET, AF_INET6 };

  for (int do_insert_sig = 0; do_insert_sig < 2; ++do_insert_sig)
    {
      insert_sig = do_insert_sig;

      /* If do_search is true, a bare host name (for example, a1-c1)
         is used.  This exercises search path processing and FQDN
         qualification.  */
      for (int do_search = 0; do_search < 2; ++do_search)
        for (const int *paf = families; paf != array_end (families); ++paf)
          {
            for (int cnames = 0; cnames <= 100; ++cnames)
              {
                check_cnames_failure (*paf, do_search, cnames, 0);
                /* Now with NXDOMAIN responses.  */
                check_cnames_failure (*paf, do_search, cnames, 255);
              }

            for (int cnames = 0; cnames <= 10; ++cnames)
              for (int addresses = 1; addresses <= 10; ++addresses)
                check (*paf, do_search, cnames, addresses);

            /* The current implementation is limited to 47 aliases.
               Addresses do not have such a limit.  */
            check (*paf, do_search, 47, 60);
          }
    }

  resolv_test_end (obj);

  return 0;
}

#include <support/test-driver.c>
