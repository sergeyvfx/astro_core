// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/horizontal.h"

#include "astro_core/coordinate/itrf.h"
#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

class HorizontalTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

TEST_F(HorizontalTest, FromITRF_Quad2) {
  // Tested against the Astropy code in horizontal_test.py, which produces the
  // following output:
  //
  //   Site : (-4680888.602721117436885834, 2805218.446534293703734875,
  //   -3292788.080450601410120726)
  //   Elevation: -43.57962307858672 deg
  //   Azimuth: 117.61108302389968 deg
  //   Distance: 46313669.89465642 m
  //
  //   Site : (4067886.640252187382429838, 571704.183895749156363308,
  //   4862789.037706432864069939)
  //   Elevation: -4.660946491068409 deg
  //   Azimuth: 265.24039353796667 deg
  //   Distance: 42201984.48655504 m
  //
  // It is close, but not as much as one would expect to the code implemented
  // here. Need to verify against ERFA.

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};

  const ITRF itrf{{.observation_time{time},
                   .position{10366753.696330964565277100,
                             -40872357.989425994455814362,
                             -7177.146307127579348162},
                   .velocity{{-0.000000123270733281970,
                              -0.000000265212595462799,
                              0.000000614554703409340}}}};

  // NOTE: Use the values returned by the algorithm during development.
  //
  // The elevation and azimuth are within 1e-3 from the Python code, but the
  // distance is much further away: it is within 16 meters.
  //
  // TODO(sergey): Figure out whether it is an issue in the RAZEL implementation
  // or whether Astropy uses different algorithm.

  {
    const ITRF site_itrf{{.observation_time{time},
                          .position{-4680888.602721117436885834,
                                    2805218.446534293703734875,
                                    -3292788.080450601410120726}}};

    const Horizontal horizontal = Horizontal::FromITRF(itrf, site_itrf);

    // Python reference:
    //
    //   Elevation: -43.57962307858672 deg
    //   Azimuth: 117.61108302389968 deg
    //   Distance: 46313669.89465642 m
    EXPECT_NEAR(
        RadiansToDegrees(horizontal.elevation), -43.579080632771798776, 1e-12);
    EXPECT_NEAR(
        RadiansToDegrees(horizontal.azimuth), 117.611381364103721126, 1e-12);
    EXPECT_NEAR(horizontal.distance, 46313685.464341290295124054, 1e-12);
  }

  {
    const ITRF site_itrf{{.observation_time{time},
                          .position{4067886.640252187382429838,
                                    571704.183895749156363308,
                                    4862789.037706432864069939}}};

    const Horizontal horizontal = Horizontal::FromITRF(itrf, site_itrf);

    // Python reference:
    //   Elevation: -4.660946491068409 deg
    //   Azimuth: 265.24039353796667 deg
    //   Distance: 42201984.48655504 m
    EXPECT_NEAR(
        RadiansToDegrees(horizontal.elevation), -4.660156591602858, 1e-12);
    EXPECT_NEAR(
        RadiansToDegrees(horizontal.azimuth), 265.240394027092122542, 1e-12);

    // According to Google Test the minimum distance for the double numbers at
    // of the given magnitude is 7.4505805969238281e-09.
    // Using an exact double match (EXPECT_DOUBLE_EQ) might be a bit fragile,
    // so instead increase the epsilon.
    EXPECT_NEAR(horizontal.distance, 42201926.333400338888168335, 8e-9);
  }
}

}  // namespace astro_core
