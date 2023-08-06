/* Single-precision vector (SVE) sin function.

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
  float poly[4];
  /* Pi-related values to be loaded as one quad-word and used with
     svmla_lane_f32.  */
  float negpi1, negpi2, negpi3, invpi;
  float shift;
} data = {
  .poly = {
    /* Non-zero coefficients from the degree 9 Taylor series expansion of
       sin.  */
    -0x1.555548p-3f, 0x1.110df4p-7f, -0x1.9f42eap-13f, 0x1.5b2e76p-19f
  },
  .negpi1 = -0x1.921fb6p+1f,
  .negpi2 = 0x1.777a5cp-24f,
  .negpi3 = 0x1.ee59dap-49f,
  .invpi = 0x1.45f306p-2f,
  .shift = 0x1.8p+23f
};

#define RangeVal 0x49800000 /* asuint32 (0x1p20f).  */
#define C(i) sv_f32 (d->poly[i])

static svfloat32_t NOINLINE
special_case (svfloat32_t x, svfloat32_t y, svbool_t cmp)
{
  return sv_call_f32 (sinf, x, y, cmp);
}

/* A fast SVE implementation of sinf.
   Maximum error: 1.89 ULPs.
   This maximum error is achieved at multiple values in [-2^18, 2^18]
   but one example is:
   SV_NAME_F1 (sin)(0x1.9247a4p+0) got 0x1.fffff6p-1 want 0x1.fffffap-1.  */
svfloat32_t SV_NAME_F1 (sin) (svfloat32_t x, const svbool_t pg)
{
  const struct data *d = ptr_barrier (&data);

  svfloat32_t ax = svabs_f32_x (pg, x);
  svuint32_t sign = sveor_u32_x (pg, svreinterpret_u32_f32 (x),
				 svreinterpret_u32_f32 (ax));
  svbool_t cmp = svcmpge_n_u32 (pg, svreinterpret_u32_f32 (ax), RangeVal);

  /* pi_vals are a quad-word of helper values - the first 3 elements contain
     -pi in extended precision, the last contains 1 / pi.  */
  svfloat32_t pi_vals = svld1rq_f32 (svptrue_b32 (), &d->negpi1);

  /* n = rint(|x|/pi).  */
  svfloat32_t n = svmla_lane_f32 (sv_f32 (d->shift), ax, pi_vals, 3);
  svuint32_t odd = svlsl_n_u32_x (pg, svreinterpret_u32_f32 (n), 31);
  n = svsub_n_f32_x (pg, n, d->shift);

  /* r = |x| - n*pi  (range reduction into -pi/2 .. pi/2).  */
  svfloat32_t r;
  r = svmla_lane_f32 (ax, n, pi_vals, 0);
  r = svmla_lane_f32 (r, n, pi_vals, 1);
  r = svmla_lane_f32 (r, n, pi_vals, 2);

  /* sin(r) approx using a degree 9 polynomial from the Taylor series
     expansion. Note that only the odd terms of this are non-zero.  */
  svfloat32_t r2 = svmul_f32_x (pg, r, r);
  svfloat32_t y;
  y = svmla_f32_x (pg, C (2), r2, C (3));
  y = svmla_f32_x (pg, C (1), r2, y);
  y = svmla_f32_x (pg, C (0), r2, y);
  y = svmla_f32_x (pg, r, r, svmul_f32_x (pg, y, r2));

  /* sign = y^sign^odd.  */
  y = svreinterpret_f32_u32 (sveor_u32_x (pg, svreinterpret_u32_f32 (y),
					  sveor_u32_x (pg, sign, odd)));

  if (__glibc_unlikely (svptest_any (pg, cmp)))
    return special_case (x, y, cmp);
  return y;
}
