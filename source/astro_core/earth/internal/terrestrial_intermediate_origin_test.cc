// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/terrestrial_intermediate_origin.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(earth, TerrestrialIntermediateOriginLocator) {
  // The reference values has been obtained using Astropy 5.3.4.

  // >>> from astropy.time import Time
  // >>> import erfa
  // >>> t = Time('2023-10-08T14:00:00.000', format='isot', scale='utc')
  // >>> t.tt.jd1, t.tt.jd2
  // (2460226.0, 0.08413407407407414)
  // >>> erfa.sp00(t.tt.jd1, t.tt.jd2)
  // -5.415723276876547e-11
  EXPECT_NEAR(TerrestrialIntermediateOriginLocator(
                  JulianDate(2460226.0, 0.08413407407407414)),
              -5.415723276876547e-11,
              1e-18);
}

}  // namespace astro_core
