/* Delete a directory.  Linux version.
   Copyright (C) 2011-2023 Free Software Foundation, Inc.
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
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#include <unistd.h>
#include <fcntl.h>
#include <sysdep.h>

/* Remove the directory PATH.  */
int
__rmdir (const char *path)
{
#ifdef __NR_rmdir
  return INLINE_SYSCALL_CALL (rmdir, path);
#else
  return INLINE_SYSCALL_CALL (unlinkat, AT_FDCWD, path, AT_REMOVEDIR);
#endif
}
weak_alias (__rmdir, rmdir)
