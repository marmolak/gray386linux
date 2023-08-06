/* Common config for EVEX256 and EVEX512 VECs
   All versions must be listed in ifunc-impl-list.c.
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

#ifndef _X86_EVEX_VECS_COMMON_H
#define _X86_EVEX_VECS_COMMON_H			1

#include "x86-vec-macros.h"

/* 6-byte mov instructions with EVEX.  */
#define MOV_SIZE			6
/* No vzeroupper needed.  */
#define RET_SIZE			1
#define VZEROUPPER

#define VMOVU				vmovdqu64
#define VMOVA				vmovdqa64
#define VMOVNT				vmovntdq

#define VMM_128				VMM_hi_xmm
#define VMM_256				VMM_hi_ymm
#define VMM_512				VMM_hi_zmm

#endif
