/* Double-precision vector (SVE) cos function.

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
  double inv_pio2, pio2_1, pio2_2, pio2_3, shift;
} data = {
  /* Polynomial coefficients are hardwired in FTMAD instructions.  */
  .inv_pio2 = 0x1.45f306dc9c882p-1,
  .pio2_1 = 0x1.921fb50000000p+0,
  .pio2_2 = 0x1.110b460000000p-26,
  .pio2_3 = 0x1.1a62633145c07p-54,
  /* Original shift used in AdvSIMD cos,
     plus a contribution to set the bit #0 of q
     as expected by trigonometric instructions.  */
  .shift = 0x1.8000000000001p52
};

#define RangeVal 0x4160000000000000 /* asuint64 (0x1p23).  */

static svfloat64_t NOINLINE
special_case (svfloat64_t x, svfloat64_t y, svbool_t out_of_bounds)
{
  return sv_call_f64 (cos, x, y, out_of_bounds);
}

/* A fast SVE implementation of cos based on trigonometric
   instructions (FTMAD, FTSSEL, FTSMUL).
   Maximum measured error: 2.108 ULPs.
   SV_NAME_D1 (cos)(0x1.9b0ba158c98f3p+7) got -0x1.fddd4c65c7f07p-3
					 want -0x1.fddd4c65c7f05p-3.  */
svfloat64_t SV_NAME_D1 (cos) (svfloat64_t x, const svbool_t pg)
{
  const struct data *d = ptr_barrier (&data);

  svfloat64_t r = svabs_f64_x (pg, x);
  svbool_t out_of_bounds
      = svcmpge_n_u64 (pg, svreinterpret_u64_f64 (r), RangeVal);

  /* Load some constants in quad-word chunks to minimise memory access.  */
  svbool_t ptrue = svptrue_b64 ();
  svfloat64_t invpio2_and_pio2_1 = svld1rq_f64 (ptrue, &d->inv_pio2);
  svfloat64_t pio2_23 = svld1rq_f64 (ptrue, &d->pio2_2);

  /* n = rint(|x|/(pi/2)).  */
  svfloat64_t q = svmla_lane_f64 (sv_f64 (d->shift), r, invpio2_and_pio2_1, 0);
  svfloat64_t n = svsub_n_f64_x (pg, q, d->shift);

  /* r = |x| - n*(pi/2)  (range reduction into -pi/4 .. pi/4).  */
  r = svmls_lane_f64 (r, n, invpio2_and_pio2_1, 1);
  r = svmls_lane_f64 (r, n, pio2_23, 0);
  r = svmls_lane_f64 (r, n, pio2_23, 1);

  /* cos(r) poly approx.  */
  svfloat64_t r2 = svtsmul_f64 (r, svreinterpret_u64_f64 (q));
  svfloat64_t y = sv_f64 (0.0);
  y = svtmad_f64 (y, r2, 7);
  y = svtmad_f64 (y, r2, 6);
  y = svtmad_f64 (y, r2, 5);
  y = svtmad_f64 (y, r2, 4);
  y = svtmad_f64 (y, r2, 3);
  y = svtmad_f64 (y, r2, 2);
  y = svtmad_f64 (y, r2, 1);
  y = svtmad_f64 (y, r2, 0);

  /* Final multiplicative factor: 1.0 or x depending on bit #0 of q.  */
  svfloat64_t f = svtssel_f64 (r, svreinterpret_u64_f64 (q));
  /* Apply factor.  */
  y = svmul_f64_x (pg, f, y);

  if (__glibc_unlikely (svptest_any (pg, out_of_bounds)))
    return special_case (x, y, out_of_bounds);
  return y;
}
