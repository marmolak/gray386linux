/* Multiple versions of memmove. AARCH64 version.
   Copyright (C) 2017-2023 Free Software Foundation, Inc.
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

/* Define multiple versions only for the definition in libc.  */

#if IS_IN (libc)
/* Redefine memmove so that the compiler won't complain about the type
   mismatch with the IFUNC selector in strong_alias, below.  */
# undef memmove
# define memmove __redirect_memmove
# include <string.h>
# include <init-arch.h>

extern __typeof (__redirect_memmove) __libc_memmove;

extern __typeof (__redirect_memmove) __memmove_generic attribute_hidden;
extern __typeof (__redirect_memmove) __memmove_thunderx attribute_hidden;
extern __typeof (__redirect_memmove) __memmove_thunderx2 attribute_hidden;
extern __typeof (__redirect_memmove) __memmove_falkor attribute_hidden;
extern __typeof (__redirect_memmove) __memmove_a64fx attribute_hidden;
extern __typeof (__redirect_memmove) __memmove_sve attribute_hidden;

static inline __typeof (__redirect_memmove) *
select_memmove_ifunc (void)
{
  INIT_ARCH ();

  if (sve && HAVE_AARCH64_SVE_ASM)
    {
      if (IS_A64FX (midr))
	return __memmove_a64fx;
      return __memmove_sve;
    }

  if (IS_THUNDERX (midr))
    return __memmove_thunderx;

  if (IS_THUNDERX2 (midr) || IS_THUNDERX2PA (midr))
    return __memmove_thunderx2;

  if (IS_FALKOR (midr) || IS_PHECDA (midr))
    return __memmove_falkor;

  return __memmove_generic;
}

libc_ifunc (__libc_memmove, select_memmove_ifunc ());

# undef memmove
strong_alias (__libc_memmove, memmove);
#endif
