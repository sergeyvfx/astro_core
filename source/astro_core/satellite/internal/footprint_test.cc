// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/footprint.h"

#include <array>

#include "astro_core/base/constants.h"
#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/geographic.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(satellite, CalculateSatelliteFootprint) {
  std::array<Geographic, 6> footprint;

  CalculateSatelliteFootprint(Geodetic({
                                  .latitude = constants::pi / 6,
                                  .longitude = constants::pi / 6,
                                  .height = 400000,
                              }),
                              footprint);

  // TODO(sergey): Find some way to better cross-reference the values.
  // Currently it is just a dump of values done by the state of algorithms which
  // seemed to provide correct results when plotting actual data on the map.

  EXPECT_NEAR(footprint[0].latitude, 0.1783365436007186, 1e-6);
  EXPECT_NEAR(footprint[0].longitude, 0.52359877559829882, 1e-6);

  EXPECT_NEAR(footprint[1].latitude, 0.52359877559829882, 1e-6);
  EXPECT_NEAR(footprint[1].longitude, 0.92294434074889153, 1e-6);

  EXPECT_NEAR(footprint[2].latitude, 0.86886100759587903, 1e-6);
  EXPECT_NEAR(footprint[2].longitude, 0.52359877559829882, 1e-6);
}

}  // namespace astro_core
