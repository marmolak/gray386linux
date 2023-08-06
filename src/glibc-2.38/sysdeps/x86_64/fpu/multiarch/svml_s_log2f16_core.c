/* Multiple versions of vectorized log2f, vector length is 16.
   Copyright (C) 2021-2023 Free Software Foundation, Inc.
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

#define SYMBOL_NAME _ZGVeN16v_log2f
#include "ifunc-mathvec-avx512-skx.h"

libc_ifunc_redirected (REDIRECT_NAME, SYMBOL_NAME, IFUNC_SELECTOR ());

#ifdef SHARED
__hidden_ver1 (_ZGVeN16v_log2f, __GI__ZGVeN16v_log2f,
	       __redirect__ZGVeN16v_log2f)
  __attribute__ ((visibility ("hidden")));
#endif
