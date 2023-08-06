/* Copyright (C) 2002-2023 Free Software Foundation, Inc.
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
#include "pthreadP.h"
#include <futex-internal.h>
#include <kernel-features.h>
#include <shlib-compat.h>

static const struct pthread_barrierattr default_barrierattr =
  {
    .pshared = PTHREAD_PROCESS_PRIVATE
  };


int
___pthread_barrier_init (pthread_barrier_t *barrier,
			const pthread_barrierattr_t *attr, unsigned int count)
{
  ASSERT_TYPE_SIZE (pthread_barrier_t, __SIZEOF_PTHREAD_BARRIER_T);
  ASSERT_PTHREAD_INTERNAL_SIZE (pthread_barrier_t,
				struct pthread_barrier);

  struct pthread_barrier *ibarrier;

  /* XXX EINVAL is not specified by POSIX as a possible error code for COUNT
     being too large.  See pthread_barrier_wait for the reason for the
     comparison with BARRIER_IN_THRESHOLD.  */
  if (__glibc_unlikely (count == 0 || count >= BARRIER_IN_THRESHOLD))
    return EINVAL;

  const struct pthread_barrierattr *iattr
    = (attr != NULL
       ? (struct pthread_barrierattr *) attr
       : &default_barrierattr);

  ibarrier = (struct pthread_barrier *) barrier;

  /* Initialize the individual fields.  */
  ibarrier->in = 0;
  ibarrier->out = 0;
  ibarrier->count = count;
  ibarrier->current_round = 0;
  ibarrier->shared = (iattr->pshared == PTHREAD_PROCESS_PRIVATE
		      ? FUTEX_PRIVATE : FUTEX_SHARED);

  return 0;
}
versioned_symbol (libc, ___pthread_barrier_init, pthread_barrier_init,
                  GLIBC_2_34);
libc_hidden_ver (___pthread_barrier_init, __pthread_barrier_init)
#ifndef SHARED
strong_alias (___pthread_barrier_init, __pthread_barrier_init)
#endif

#if OTHER_SHLIB_COMPAT (libpthread, GLIBC_2_2, GLIBC_2_34)
compat_symbol (libpthread, ___pthread_barrier_init, pthread_barrier_init,
               GLIBC_2_2);
#endif
