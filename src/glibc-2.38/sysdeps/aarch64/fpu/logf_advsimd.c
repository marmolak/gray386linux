/* Single-precision vector (Advanced SIMD) log function.

   Copyright (C) 2023 Free Software Foundation, Inc.
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

#include "v_math.h"

static const struct data
{
  float32x4_t poly[7];
  float32x4_t ln2, tiny_bound;
  uint32x4_t min_norm, special_bound, off, mantissa_mask;
} data = {
  /* 3.34 ulp error.  */
  .poly = { V4 (-0x1.3e737cp-3f), V4 (0x1.5a9aa2p-3f), V4 (-0x1.4f9934p-3f),
	    V4 (0x1.961348p-3f), V4 (-0x1.00187cp-2f), V4 (0x1.555d7cp-2f),
	    V4 (-0x1.ffffc8p-2f) },
  .ln2 = V4 (0x1.62e43p-1f),
  .tiny_bound = V4 (0x1p-126),
  .min_norm = V4 (0x00800000),
  .special_bound = V4 (0x7f000000), /* asuint32(inf) - min_norm.  */
  .off = V4 (0x3f2aaaab),	    /* 0.666667.  */
  .mantissa_mask = V4 (0x007fffff)
};

#define P(i) d->poly[7 - i]

static float32x4_t VPCS_ATTR NOINLINE
special_case (float32x4_t x, float32x4_t y, uint32x4_t cmp)
{
  /* Fall back to scalar code.  */
  return v_call_f32 (logf, x, y, cmp);
}

float32x4_t VPCS_ATTR V_NAME_F1 (log) (float32x4_t x)
{
  const struct data *d = ptr_barrier (&data);
  float32x4_t n, p, q, r, r2, y;
  uint32x4_t u, cmp;

  u = vreinterpretq_u32_f32 (x);
  cmp = vcgeq_u32 (vsubq_u32 (u, d->min_norm), d->special_bound);

  /* x = 2^n * (1+r), where 2/3 < 1+r < 4/3.  */
  u = vsubq_u32 (u, d->off);
  n = vcvtq_f32_s32 (
      vshrq_n_s32 (vreinterpretq_s32_u32 (u), 23)); /* signextend.  */
  u = vandq_u32 (u, d->mantissa_mask);
  u = vaddq_u32 (u, d->off);
  r = vsubq_f32 (vreinterpretq_f32_u32 (u), v_f32 (1.0f));

  /* y = log(1+r) + n*ln2.  */
  r2 = vmulq_f32 (r, r);
  /* n*ln2 + r + r2*(P1 + r*P2 + r2*(P3 + r*P4 + r2*(P5 + r*P6 + r2*P7))).  */
  p = vfmaq_f32 (P (5), P (6), r);
  q = vfmaq_f32 (P (3), P (4), r);
  y = vfmaq_f32 (P (1), P (2), r);
  p = vfmaq_f32 (p, P (7), r2);
  q = vfmaq_f32 (q, p, r2);
  y = vfmaq_f32 (y, q, r2);
  p = vfmaq_f32 (r, d->ln2, n);
  y = vfmaq_f32 (p, y, r2);

  if (__glibc_unlikely (v_any_u32 (cmp)))
    return special_case (x, y, cmp);
  return y;
}
