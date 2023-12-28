// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/geodetic.h"

#include "astro_core/coordinate/geographic.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(Geodetic, Construct) {
  const DateTime date_time(2022, 8, 10, 22, 0, 0);
  const Time time{date_time, TimeScale::kUTC};

  const Geodetic coordinate(
      {.observation_time{time}, .latitude = 1, .longitude = 2, .height = 3});

  EXPECT_EQ(coordinate.latitude, 1);
  EXPECT_EQ(coordinate.longitude, 2);
  EXPECT_EQ(coordinate.height, 3);
  EXPECT_EQ(coordinate.observation_time.AsFormat<DateTime>(), date_time);
}

// Test construction of the geodetic coordinate from cartesian.
//
// Verified against the naive implementation using ERFA:
//
//   #include <stdio.h>
//
//   #include "erfa.h"
//   #include "erfam.h"
//
//   int main() {
//     double xyz[3] = {-2801.4282006655326e+3,
//                      5602.703303257319e+3,
//                      -2645.0940902936873e+3};
//
//     double longitude = 0, latitude = 0, height = 0;
//     eraGc2gd(ERFA_WGS84, xyz, &longitude, &latitude, &height);
//
//     printf("%.18f %.18f %.18f\n", longitude, latitude, height);
//
//     return 0;
//   }
//
//   $ clang main.c gc2gd.c eform.c gc2gde.c
//   $ ./a.out
//   2.034454866036249587 -0.401814025006804576 424729.911998023919295520
TEST(Geodetic, FromGeocentric) {
  Time time;

  // Exact north pole.
  {
    const Geodetic gd = Geodetic::FromGeocentric(Vec3{0, 0, 10000000.0}, time);
    EXPECT_NEAR(gd.longitude, 0, 1e-12);
    EXPECT_NEAR(gd.latitude, 1.570796326794896558, 1e-12);
    EXPECT_NEAR(gd.height, 3643247.685754820704460144, 1e-12);
  }

  // Exact south pole.
  {
    const Geodetic gd = Geodetic::FromGeocentric(Vec3{0, 0, -10000000.0}, time);
    EXPECT_NEAR(gd.longitude, 0, 1e-12);
    EXPECT_NEAR(gd.latitude, -1.570796326794896558, 1e-12);
    EXPECT_NEAR(gd.height, 3643247.685754820704460144, 1e-12);
  }

  {
    const Geodetic gd = Geodetic::FromGeocentric(Vec3{-2801.4282006655326e+3,
                                                      5602.703303257319e+3,
                                                      -2645.0940902936873e+3},
                                                 time);
    EXPECT_NEAR(gd.longitude, 2.034454866036249587, 1e-12);
    EXPECT_NEAR(gd.latitude, -0.401814025006804576, 1e-12);

    // The height is differs by more than 1e-12.
    // The ERFA seems to be using different formula than the one presented in
    // the [Fukushima2006].
    EXPECT_NEAR(gd.height, 424729.911998023919295520, 1e-8);
  }
}

TEST(Geodetic, FromGeographic) {
  Time time;

  // THe data is obtained from Astropy for the Siding Spring observatory.
  //
  //   >>> from astropy.coordinates import EarthLocation
  //   >>> print(EarthLocation.of_site("aao").geodetic)
  //   GeodeticLocation(lon=<Longitude 149.06608611 deg>,
  //   lat=<Latitude -31.27703889 deg>, height=<Quantity 1164. m>)
  const Geodetic gd =
      Geodetic::FromGeographic(Geographic({
                                   .latitude = DegreesToRadians(-31.27703889),
                                   .longitude = DegreesToRadians(149.06608611),
                               }),
                               time);

  EXPECT_NEAR(gd.latitude, DegreesToRadians(-31.27703889), 1e-12);
  EXPECT_NEAR(gd.longitude, DegreesToRadians(149.06608611), 1e-12);
  EXPECT_NEAR(gd.height, 0.0, 1e-12);
}

}  // namespace astro_core
