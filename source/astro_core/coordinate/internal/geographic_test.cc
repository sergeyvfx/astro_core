// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/geographic.h"

#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/qth.h"
#include "astro_core/math/math.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(Geographic, FromGeodetic) {
  Time time;

  const Geodetic geodetic({
      .observation_time = time,
      .latitude = DegreesToRadians(-31.27703889),
      .longitude = DegreesToRadians(149.06608611),
      .height = 100,
  });

  const Geographic geographic = Geographic::FromGeodetic(geodetic);

  EXPECT_NEAR(geographic.latitude, DegreesToRadians(-31.27703889), 1e-12);
  EXPECT_NEAR(geographic.longitude, DegreesToRadians(149.06608611), 1e-12);
}

TEST(Geographic, FromQTH) {
  const Geographic geographic = Geographic::FromQTH(QTH("BL11bh16"));

  EXPECT_NEAR(RadiansToDegrees(geographic.latitude), 21.3166666666667, 1e-8);
  EXPECT_NEAR(RadiansToDegrees(geographic.longitude), -157.908333333333, 1e-8);
}

}  // namespace astro_core
