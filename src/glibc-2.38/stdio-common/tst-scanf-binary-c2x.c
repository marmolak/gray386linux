/* Test scanf functions with C2X binary integers (narrow strings,
   no extensions).
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

/* Some versions of GCC supported for building glibc do not support
   -std=c2x.  */
#undef _GNU_SOURCE
#define _ISOC2X_SOURCE

#define CHAR char
#define FNW
#define L_(C) C
#define TEST_C2X 1
#define STD "c2x"

#include <tst-scanf-binary-main.c>
