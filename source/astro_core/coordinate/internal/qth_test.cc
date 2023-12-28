// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/qth.h"

#include "astro_core/math/math.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(QTH, Construct) {
  EXPECT_STREQ(QTH("BL11bh16").GetLocator(), "BL11bh16");
  EXPECT_STREQ(QTH("bl11BH16").GetLocator(), "BL11bh16");
}

TEST(QTH, CalculateLatitude) {
  EXPECT_NEAR(RadiansToDegrees(QTH("BL11bh16").CalculateLatitude()),
              21.3166666666667,
              1e-8);
}

TEST(QTH, CalculateLongitude) {
  EXPECT_NEAR(RadiansToDegrees(QTH("BL11bh16").CalculateLongitude()),
              -157.908333333333,
              1e-8);
}

}  // namespace astro_core
