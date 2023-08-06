/* Auxiliary functions for tst-audit24x.
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

#ifndef _TST_AUDITMOD24_H
#define _TST_AUDITMOD24_H

static void
test_symbind_flags (unsigned int flags)
{
  if ((flags & (LA_SYMB_NOPLTENTER | LA_SYMB_NOPLTEXIT)) == 0)
    abort ();
}

#endif
