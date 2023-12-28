// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/pass.h"

#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/geographic.h"
#include "astro_core/coordinate/itrf.h"
#include "astro_core/math/math.h"
#include "astro_core/satellite/orbital_state.h"
#include "astro_core/satellite/tle.h"
#include "astro_core/satellite/tle_parser.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

class PassTest : public testing::Test {
 protected:
  auto CreateOrbitalStateFromTLE(const std::string_view line1,
                                 const std::string_view line2) -> OrbitalState {
    const TLEParser::Result result = TLEParser::FromLines(line1, line2);
    EXPECT_TRUE(result.Ok());

    OrbitalState orbital_state;
    EXPECT_TRUE(orbital_state.InitializeFromTLE(result.GetValue()));

    return orbital_state;
  }
};

TEST_F(PassTest, PredictCurrentOrNextPass_PolarOrbiting) {
  const OrbitalState orbital_state = CreateOrbitalStateFromTLE(
      "1 25338U 98030A   22362.54560834  .00000123  00000+0  69546-4 0  9990",
      "2 25338  98.6255  29.3628 0011429  91.9881 268.2609 14.26213421280684");

  const ITRF site_position = ITRF::FromGeodetic(
      Geodetic::FromGeographic(Geographic({
                                   .latitude = DegreesToRadians(50.0),
                                   .longitude = DegreesToRadians(5.0),
                               }),
                               Time{DateTime(2022, 12, 28), TimeScale::kUTC}));

  // The satellite at this time is over the observer, with AOS 18:15:25, LOS
  // 18:30:35, and the maximum elevation of 82.9 degrees.
  //
  // It is expected that this pass is ignored and the next one is picked as it
  // will be the first one to cross the 20 degrees maximum elevation.
  {
    const Time start_time{DateTime(2022, 12, 28, 18, 20), TimeScale::kUTC};

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(20.0),
    };

    const SatellitePass pass =
        PredictCurrentOrNextPass(options, orbital_state, start_time);

    EXPECT_FALSE(pass.is_always_visible);
    EXPECT_FALSE(pass.is_never_visible);

    ASSERT_TRUE(pass.aos);
    ASSERT_TRUE(pass.los);

    EXPECT_EQ(pass.aos->AsFormat<DateTime>(),
              DateTime(2022, 12, 28, 18, 15, 25));
    EXPECT_EQ(pass.los->AsFormat<DateTime>(),
              DateTime(2022, 12, 28, 18, 30, 35));
    EXPECT_NEAR(pass.max_elevation, DegreesToRadians(82.94732027103807), 1e-8);
  }

  // The satellite at this time is over the observer, with AOS 16:37:40, LOS
  // 16:50:17, and the maximum elevation of 16.3 degrees.
  //
  // It is expected that this pass is ignored and the next one is picked as it
  // will be the first one to cross the 20 degrees maximum elevation.
  {
    const Time start_time{DateTime(2022, 12, 28, 16, 40), TimeScale::kUTC};

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(20.0),
    };

    const SatellitePass pass =
        PredictCurrentOrNextPass(options, orbital_state, start_time);

    EXPECT_FALSE(pass.is_always_visible);
    EXPECT_FALSE(pass.is_never_visible);

    ASSERT_TRUE(pass.aos);
    ASSERT_TRUE(pass.los);

    EXPECT_EQ(pass.aos->AsFormat<DateTime>(),
              DateTime(2022, 12, 28, 18, 15, 25));
    EXPECT_EQ(pass.los->AsFormat<DateTime>(),
              DateTime(2022, 12, 28, 18, 30, 35));
    EXPECT_NEAR(pass.max_elevation, DegreesToRadians(82.94732027103807), 1e-8);
  }
}

TEST_F(PassTest, PredictCurrentOrNextPass_Geostationary) {
  const OrbitalState orbital_state = CreateOrbitalStateFromTLE(
      "1 41866U 16071A   22363.53313590 -.00000245  00000-0  00000+0 0  9996",
      "2 41866   0.0752 249.2647 0000691  59.7958 265.6365  1.00271109 22428");

  // Site is outside the coverage.
  {
    const Time start_time{DateTime(2022, 12, 28, 18, 20), TimeScale::kUTC};
    const ITRF site_position = ITRF::FromGeodetic(
        Geodetic::FromGeographic(Geographic({
                                     .latitude = DegreesToRadians(50.0),
                                     .longitude = DegreesToRadians(5.0),
                                 }),
                                 start_time));

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(10.0),
    };
    const SatellitePass pass =
        PredictCurrentOrNextPass(options, orbital_state, start_time);

    EXPECT_FALSE(pass.is_always_visible);
    EXPECT_TRUE(pass.is_never_visible);
  }

  // Site is inside the coverage.
  {
    const Time start_time{DateTime(2022, 12, 28, 18, 20), TimeScale::kUTC};
    const ITRF site_position = ITRF::FromGeodetic(
        Geodetic::FromGeographic(Geographic({
                                     .latitude = DegreesToRadians(0.0),
                                     .longitude = DegreesToRadians(-75.0),
                                 }),
                                 start_time));

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(10.0),
    };
    const SatellitePass pass =
        PredictCurrentOrNextPass(options, orbital_state, start_time);

    EXPECT_TRUE(pass.is_always_visible);
    EXPECT_FALSE(pass.is_never_visible);

    EXPECT_NEAR(pass.max_elevation, DegreesToRadians(89.79408886501358), 1e-8);
  }
}

TEST_F(PassTest, PredictNextPass_PolarOrbiting) {
  const OrbitalState orbital_state = CreateOrbitalStateFromTLE(
      "1 25338U 98030A   22362.54560834  .00000123  00000+0  69546-4 0  9990",
      "2 25338  98.6255  29.3628 0011429  91.9881 268.2609 14.26213421280684");

  const ITRF site_position = ITRF::FromGeodetic(
      Geodetic::FromGeographic(Geographic({
                                   .latitude = DegreesToRadians(50.0),
                                   .longitude = DegreesToRadians(5.0),
                               }),
                               Time{DateTime(2022, 12, 28), TimeScale::kUTC}));

  // The satellite at this time is over the observer, with AOS 16:37:40, LOS
  // 16:50:17, and the maximum elevation of 16.3 degrees.
  //
  // It is expected that this pass is ignored since current pass is requested to
  // be ignored.
  {
    const Time start_time{DateTime(2022, 12, 28, 16, 40), TimeScale::kUTC};

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(10.0),
    };

    const SatellitePass pass =
        PredictNextPass(options, orbital_state, start_time);

    EXPECT_FALSE(pass.is_always_visible);
    EXPECT_FALSE(pass.is_never_visible);

    ASSERT_TRUE(pass.aos);
    ASSERT_TRUE(pass.los);

    EXPECT_EQ(pass.aos->AsFormat<DateTime>(),
              DateTime(2022, 12, 28, 18, 15, 25));
    EXPECT_EQ(pass.los->AsFormat<DateTime>(),
              DateTime(2022, 12, 28, 18, 30, 35));
    EXPECT_NEAR(pass.max_elevation, DegreesToRadians(82.94732027103807), 1e-8);
  }
}

TEST_F(PassTest, PredictNextPass_Geostationary) {
  const OrbitalState orbital_state = CreateOrbitalStateFromTLE(
      "1 41866U 16071A   22363.53313590 -.00000245  00000-0  00000+0 0  9996",
      "2 41866   0.0752 249.2647 0000691  59.7958 265.6365  1.00271109 22428");

  // Site is outside the coverage.
  {
    const Time start_time{DateTime(2022, 12, 28, 18, 20), TimeScale::kUTC};
    const ITRF site_position = ITRF::FromGeodetic(
        Geodetic::FromGeographic(Geographic({
                                     .latitude = DegreesToRadians(50.0),
                                     .longitude = DegreesToRadians(5.0),
                                 }),
                                 start_time));

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(10.0),
    };
    const SatellitePass pass =
        PredictNextPass(options, orbital_state, start_time);

    EXPECT_FALSE(pass.is_always_visible);
    EXPECT_TRUE(pass.is_never_visible);
  }

  // Site is inside the coverage, but below the minimal elevation.
  {
    const Time start_time{DateTime(2022, 12, 28, 18, 20), TimeScale::kUTC};
    const ITRF site_position = ITRF::FromGeodetic(
        Geodetic::FromGeographic(Geographic({
                                     .latitude = DegreesToRadians(0.0),
                                     .longitude = DegreesToRadians(0.0),
                                 }),
                                 start_time));

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(16.0),
    };
    const SatellitePass pass =
        PredictNextPass(options, orbital_state, start_time);

    EXPECT_FALSE(pass.is_always_visible);
    EXPECT_TRUE(pass.is_never_visible);
  }

  // Site is inside the coverage.
  {
    const Time start_time{DateTime(2022, 12, 28, 18, 20), TimeScale::kUTC};
    const ITRF site_position = ITRF::FromGeodetic(
        Geodetic::FromGeographic(Geographic({
                                     .latitude = DegreesToRadians(0.0),
                                     .longitude = DegreesToRadians(-75.0),
                                 }),
                                 start_time));

    const PredictPassOptions options = {
        .site_position = site_position,
        .min_elevation = DegreesToRadians(10.0),
    };
    const SatellitePass pass =
        PredictNextPass(options, orbital_state, start_time);

    EXPECT_TRUE(pass.is_always_visible);
    EXPECT_FALSE(pass.is_never_visible);

    EXPECT_FALSE(pass.aos);
    EXPECT_FALSE(pass.los);

    EXPECT_NEAR(pass.max_elevation, DegreesToRadians(89.79408886501358), 1e-8);
  }
}

}  // namespace astro_core
