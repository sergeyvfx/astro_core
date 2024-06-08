// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/itrf.h"

#include "astro_core/coordinate/gcrf.h"
#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/teme.h"
#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

class ITRFTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

TEST_F(ITRFTest, Construct) {
  const DateTime date_time(2022, 8, 10, 22, 0, 0);
  const Time time{date_time, TimeScale::kUTC};

  const ITRF coordinate(
      {.observation_time{time}, .position{1, 2, 3}, .velocity{{4, 5, 6}}});

  EXPECT_EQ(coordinate.position.GetCartesian(), Vec3(1, 2, 3));
  ASSERT_TRUE(coordinate.velocity.HasValue());
  EXPECT_EQ(coordinate.velocity.GetCartesian(), Vec3(4, 5, 6));
  EXPECT_EQ(coordinate.observation_time.AsFormat<DateTime>(), date_time);
}

TEST_F(ITRFTest, FromGCRF) {
  // Test follows the test suit for GCRFToITRF().

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_gcrf(4374.025673658524283383,
                    4478.288319286147270759,
                    -2654.739186783237528289);
  const Vec3 v_gcrf(
      -2.139329590299860584, 5.174189009638810788, 5.220516738855706329);

  const GCRF gcrf(
      {.observation_time = time, .position = r_gcrf, .velocity = v_gcrf});

  const ITRF itrf = ITRF::FromGCRF(gcrf);

  EXPECT_EQ(itrf.observation_time, gcrf.observation_time);

  EXPECT_THAT(Vec3(itrf.position.GetCartesian()),
              Pointwise(DoubleNear(1e-12),
                        {-2801.428206798944302136,
                         5602.703300938050233526,
                         -2645.094088710325195279}));

  ASSERT_TRUE(itrf.velocity.HasValue());
  EXPECT_THAT(Vec3(itrf.velocity.GetCartesian()),
              Pointwise(DoubleNear(1e-12),
                        {-5.184234346857372167,
                         -0.137714270932494498,
                         5.215984905464025267}));
}

TEST_F(ITRFTest, FromTEME) {
  // Test follows the test suit for TEMEToITRF().

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_teme{4357.092619856639, 4500.439126822302, -2645.108425391841};
  const Vec3 v_teme{-2.1768117558889037, 5.163121595591936, 5.215977759982141};

  const TEME teme(
      {.observation_time = time, .position = r_teme, .velocity = v_teme});

  const ITRF itrf = ITRF::FromTEME(teme);

  EXPECT_EQ(itrf.observation_time, teme.observation_time);

  EXPECT_THAT(Vec3(itrf.position.GetCartesian()),
              Pointwise(DoubleNear(1e-12),
                        {-2801.428206798944302136,
                         5602.703300938050233526,
                         -2645.094088710325195279}));

  ASSERT_TRUE(itrf.velocity.HasValue());
  EXPECT_THAT(Vec3(itrf.velocity.GetCartesian()),
              Pointwise(DoubleNear(1e-12),
                        {-5.184234346857372167,
                         -0.137714270932494498,
                         5.215984905464025267}));
}

////////////////////////////////////////////////////////////////////////////////
// Geodetic to geocentric.
//
// Using the following code to generate the test reference:
//
//   #include <stdio.h>
//   #include <math.h>
//
//   #include "erfa.h"
//   #include "erfam.h"
//
//   double DegreesToRadians(const double value) {
//     return value * M_PI / 180.0;
//   }
//
//   int main() {
//     const double longitude = DegreesToRadians(149.06608611111113);
//     const double latitude = DegreesToRadians(-31.277038888888896);
//     const double height = 1164.0000000016162;
//
//     double xyz[3];
//
//     eraGd2gc(ERFA_WGS84, longitude, latitude, height, xyz);
//
//     printf("%.18f %.18f %.18f\n", xyz[0], xyz[1], xyz[2]);
//
//     return 0;
//   }
//   $ clang -Wall -Wextra main.c gd2gc.c gd2gce.c eform.c zp.c
//   $ ./a.out
//   -4680888.602721118368208408 2805218.446534294169396162
//   -3292788.080450602807104588
//
// The corresponding Python code:
//
//   >>> import numpy as np
//   >>> from astropy.coordinates import EarthLocation
//   >>> np.set_printoptions(precision=18, suppress=True)
//   >>> geodetic = EarthLocation.of_site("aao")
//   >>> print(f"{geodetic.itrs.earth_location.x.value:0.18f} "
//   >>>       f"{geodetic.itrs.earth_location.y.value:0.18f} "
//   >>>       f"{geodetic.itrs.earth_location.z.value:0.18f}")
//   -4680888.602721117436885834 2805218.446534293703734875
//   -3292788.080450601410120726
//
// The Python code gives slightly different result. It might be explained by
// the amount of time the values are wrapped between different representations.

TEST_F(ITRFTest, FromGeodetic) {
  // Siding Spring observatory. Obtained from Astropy:
  //
  //   >>> import numpy as np
  //   >>> from astropy.coordinates import EarthLocation
  //   >>> np.set_printoptions(precision=18, suppress=True)
  //   >>> siding_spring = EarthLocation.of_site("aao")
  //   >>> siding_spring.geodetic
  //   GeodeticLocation(lon=<Longitude 149.06608611111113 deg>,
  //       lat=<Latitude -31.277038888888896 deg>,
  //       height=<Quantity 1164.0000000016162 m>)
  const Geodetic geodetic({
      .latitude = DegreesToRadians(-31.277038888888896),
      .longitude = DegreesToRadians(149.06608611111113),
      .height = 1164.0000000016162,
  });

  const ITRF itrf = ITRF::FromGeodetic(geodetic);

  EXPECT_THAT(Vec3(itrf.position.GetCartesian()),
              Pointwise(DoubleNear(1e-12),
                        {-4680888.602721118368208408,
                         2805218.446534294169396162,
                         -3292788.080450602807104588}));

  EXPECT_FALSE(itrf.velocity.HasValue());

  EXPECT_EQ(itrf.observation_time.AsFormat<DateTime>(),
            geodetic.observation_time.AsFormat<DateTime>());
}

}  // namespace astro_core
