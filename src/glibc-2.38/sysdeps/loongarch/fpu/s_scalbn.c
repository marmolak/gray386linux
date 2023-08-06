/* scalbn().  LoongArch version.
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

#define NO_MATH_REDIRECT
#include <math.h>

double
__scalbn (double x, int fn)
{
  double tmp;
  asm volatile ("movgr2fr.d \t%0, %1" : "=f" (tmp) : "r" (fn));
  asm volatile ("fscaleb.d \t%0, %1, %2" : "=f" (x) : "f" (x), "f" (tmp));
  return x;
}
