// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/nutation.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(earth, CalculateNutation00A) {
  // >>> from astropy.time import Time
  // >>> time = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  // >>> t = (time.tt.jd1 - 2451545 + time.tt.jd2)/36525
  // >>> time.tt.jd1, time.tt.jd2
  // (2459496.0, -0.4991992592592593)

  const Nutation nutation =
      CalculateNutation00A(JulianDate(2459496.0, -0.4991992592592593));

  // >>> import erfa
  // >>> erfa.nut00a(2459496.0, -0.4991992592592593)
  // (-7.951329720027811e-05, 2.2442420308558367e-05)
  EXPECT_NEAR(nutation.dpsi, -7.951329720027811e-05, 1e-10);
  EXPECT_NEAR(nutation.deps, 2.2442420308558367e-05, 1e-10);

  // Validate using higher precision, using self-verified value.
  EXPECT_NEAR(nutation.dpsi, -0.000079513284058339, 1e-16);
  EXPECT_NEAR(nutation.deps, 0.000022442406043647, 1e-16);
}

TEST(earth, CalculateNutation06) {
  // >>> from astropy.time import Time
  // >>> time = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  // >>> t = (time.tt.jd1 - 2451545 + time.tt.jd2)/36525
  // >>> time.tt.jd1, time.tt.jd2
  // (2459496.0, -0.4991992592592593)

  const Nutation nutation =
      CalculateNutation06A(JulianDate(2459496.0, -0.4991992592592593));

  // >>> import erfa
  // >>> erfa.nut06a(2459496.0, -0.4991992592592593)
  // (-7.951328647675147e-05, 2.244240674066618e-05)
  EXPECT_NEAR(nutation.dpsi, -7.951328647675147e-05, 1e-10);
  EXPECT_NEAR(nutation.deps, 2.244240674066618e-05, 1e-10);

  // Validate using higher precision, using self-verified value.
  EXPECT_NEAR(nutation.dpsi, -0.000079513273334814, 1e-16);
  EXPECT_NEAR(nutation.deps, 0.000022442392475763, 1e-16);
}

}  // namespace astro_core
