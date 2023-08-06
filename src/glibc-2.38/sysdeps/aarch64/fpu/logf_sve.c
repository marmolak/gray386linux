/* Single-precision vector (SVE) log function.

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

#include "sv_math.h"

static const struct data
{
  float poly_0135[4];
  float poly_246[3];
  float ln2;
} data = {
  .poly_0135 = {
    /* Coefficients copied from the AdvSIMD routine in math/, then rearranged so
       that coeffs 0, 1, 3 and 5 can be loaded as a single quad-word, hence used
       with _lane variant of MLA intrinsic.  */
    -0x1.3e737cp-3f, 0x1.5a9aa2p-3f, 0x1.961348p-3f, 0x1.555d7cp-2f
  },
  .poly_246 = { -0x1.4f9934p-3f, -0x1.00187cp-2f, -0x1.ffffc8p-2f },
  .ln2 = 0x1.62e43p-1f
};

#define Min (0x00800000)
#define Max (0x7f800000)
#define Thresh (0x7f000000) /* Max - Min.  */
#define Mask (0x007fffff)
#define Off (0x3f2aaaab) /* 0.666667.  */

static svfloat32_t NOINLINE
special_case (svfloat32_t x, svfloat32_t y, svbool_t cmp)
{
  return sv_call_f32 (logf, x, y, cmp);
}

/* Optimised implementation of SVE logf, using the same algorithm and
   polynomial as the AdvSIMD routine. Maximum error is 3.34 ULPs:
   SV_NAME_F1 (log)(0x1.557298p+0) got 0x1.26edecp-2
				  want 0x1.26ede6p-2.  */
svfloat32_t SV_NAME_F1 (log) (svfloat32_t x, const svbool_t pg)
{
  const struct data *d = ptr_barrier (&data);

  svuint32_t u = svreinterpret_u32_f32 (x);
  svbool_t cmp = svcmpge_n_u32 (pg, svsub_n_u32_x (pg, u, Min), Thresh);

  /* x = 2^n * (1+r), where 2/3 < 1+r < 4/3.  */
  u = svsub_n_u32_x (pg, u, Off);
  svfloat32_t n
      = svcvt_f32_s32_x (pg, svasr_n_s32_x (pg, svreinterpret_s32_u32 (u),
					    23)); /* Sign-extend.  */
  u = svand_n_u32_x (pg, u, Mask);
  u = svadd_n_u32_x (pg, u, Off);
  svfloat32_t r = svsub_n_f32_x (pg, svreinterpret_f32_u32 (u), 1.0f);

  /* y = log(1+r) + n*ln2.  */
  svfloat32_t r2 = svmul_f32_x (pg, r, r);
  /* n*ln2 + r + r2*(P6 + r*P5 + r2*(P4 + r*P3 + r2*(P2 + r*P1 + r2*P0))).  */
  svfloat32_t p_0135 = svld1rq_f32 (svptrue_b32 (), &d->poly_0135[0]);
  svfloat32_t p = svmla_lane_f32 (sv_f32 (d->poly_246[0]), r, p_0135, 1);
  svfloat32_t q = svmla_lane_f32 (sv_f32 (d->poly_246[1]), r, p_0135, 2);
  svfloat32_t y = svmla_lane_f32 (sv_f32 (d->poly_246[2]), r, p_0135, 3);
  p = svmla_lane_f32 (p, r2, p_0135, 0);

  q = svmla_f32_x (pg, q, r2, p);
  y = svmla_f32_x (pg, y, r2, q);
  p = svmla_n_f32_x (pg, r, n, d->ln2);
  y = svmla_f32_x (pg, p, r2, y);

  if (__glibc_unlikely (svptest_any (pg, cmp)))
    return special_case (x, y, cmp);
  return y;
}
