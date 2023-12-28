// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/epoch_convert.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

TEST(time, JulianCenturiesSinceJ2000) {
  EXPECT_NEAR(
      double(JulianCenturiesSinceJ2000(JulianDate{2451545.0})), 0.0, 1e-12);

  EXPECT_NEAR(double(JulianCenturiesSinceJ2000(JulianDate{2451545.0 + 365.25})),
              0.01,
              1e-12);

  EXPECT_THAT(
      DoubleDouble(JulianCenturiesSinceJ2000(
          JulianDate{2459802.416666320059448481, 0.000000000114644849})),
      NearUsingAbsDifferenceMetric(
          DoubleDouble(0.226075747195624205, 0.000000000000000000), 1e-12));
}

}  // namespace astro_core
