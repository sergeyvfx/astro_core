// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/doppler.h"

#include "astro_core/coordinate/itrf.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(satellite, CalculateDopplerFactor) {
  const ITRF site({
      .position = Vec3(0, 0, 0),
      .velocity = Vec3(0, 0, 0),
  });

  const ITRF satellite({
      .position = Vec3(1000, 0, 0),
      .velocity = Vec3(-30000, 0, 0),
  });

  EXPECT_NEAR(
      CalculateDopplerFactor(site, satellite), 1.000100079243412, 1e-12);
}

}  // namespace astro_core
