/* Debugging support.  MIPS version.
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

#ifndef _DL_DEBUG_H
#define _DL_DEBUG_H

/* If there is a DT_MIPS_RLD_MAP_REL or DT_MIPS_RLD_MAP entry in the
   dynamic section, fill in the debug map pointer with the run-time
   address of the r_debug structure.  */

static inline void
__attribute ((always_inline))
elf_setup_debug_entry (struct link_map *l, struct r_debug *r)
{
  if (l->l_info[DT_MIPS (RLD_MAP_REL)] != NULL)
    {
      char *ptr = (char *) l->l_info[DT_MIPS (RLD_MAP_REL)];
      ptr += l->l_info[DT_MIPS (RLD_MAP_REL)]->d_un.d_val;
      *(ElfW(Addr) *) ptr = (ElfW(Addr)) r;
    }
  else if (l->l_info[DT_MIPS (RLD_MAP)] != NULL)
    *(ElfW(Addr) *) (l->l_info[DT_MIPS (RLD_MAP)]->d_un.d_ptr)
      = (ElfW(Addr)) r;
}

#endif /* _DL_DEBUG_H */
