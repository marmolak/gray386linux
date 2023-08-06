/* Convert string representing a number to integer value, using given locale.
   Copyright (C) 1997-2023 Free Software Foundation, Inc.
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

#define UNSIGNED	1

#include <features.h>
#undef __GLIBC_USE_C2X_STRTOL
#define __GLIBC_USE_C2X_STRTOL 0
#include <locale.h>
#include <stdbool.h>

extern unsigned long int ____strtoul_l_internal (const char *, char **, int,
						 int, bool, locale_t);

#include "strtol_l.c"
