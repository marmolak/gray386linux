/* wcsncmp dispatch for RTLD and non-multiarch .c ISA level 1 build.
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

/* wcsncmp non-multiarch build is split into two files,
   wcsncmp-generic.c and wcsncmp.S. The wcsncmp-generic.c build is for
   ISA level <= 1 and just uses wcsmbs/wcsncmp.c.  This must be split
   into two files because we cannot include C code from assembly or
   vice versa.  */

#include <isa-level.h>

#if MINIMUM_X86_ISA_LEVEL <= 2
# include "wcsmbs/wcsncmp.c"
#endif
