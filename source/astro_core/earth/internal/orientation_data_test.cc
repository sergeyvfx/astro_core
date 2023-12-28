// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation_data.h"

#include "astro_core/earth/internal/orientation_test_data.h"
#include "astro_core/earth/orientation_table.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

namespace {

auto MakeEarthOrientationData() -> EarthOrientationData {
  EarthOrientationData earth_orientation_data;

  earth_orientation_data.SetTable(test_data::CreateEarthOrientationTable());

  return earth_orientation_data;
}

}  // namespace

TEST(EarthOrientationData, Lookup_UT1_Minus_UTC_UTC) {
  const EarthOrientationData earth_orientation_data =
      MakeEarthOrientationData();

  // The reference values has been obtained using Astropy 5.3.4.

  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> iers.conf.auto_download = False
  // >>> Time(50814, format='mjd').get_delta_ut1_utc().value
  // 0.2181283
  EXPECT_NEAR(earth_orientation_data.LookupUT1MinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(50814.0)),
              0.2181283,
              1e-12);

  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> iers.conf.auto_download = False
  // >>> Time(50815, format='mjd').get_delta_ut1_utc().value
  // 0.2165438
  EXPECT_NEAR(earth_orientation_data.LookupUT1MinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(50815.0)),
              0.2165438,
              1e-12);

  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> iers.conf.auto_download = False
  // >>> Time(50814.1, format='mjd').get_delta_ut1_utc().value
  // 0.2179698500000023
  EXPECT_NEAR(earth_orientation_data.LookupUT1MinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(50814.1)),
              0.2179698500000023,
              1e-12);
}

TEST(EarthOrientationData, Lookup_PolarMotion_UTC) {
  const EarthOrientationData earth_orientation_data =
      MakeEarthOrientationData();

  // The reference values has been obtained using Astropy 5.3.4.

  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  // >>> t = Time(50814, format='mjd', scale='utc')
  // >>> [a.value for a in iers_b.pm_xy(t.jd1, t.jd2)]
  // [0.102366, 0.174641]
  EXPECT_THAT(earth_orientation_data.LookupPolarMotionArcsecInUTCScale(
                  ModifiedJulianDate(50814.0)),
              Pointwise(DoubleNear(1e-12), {0.102366, 0.174641}));

  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> t = Time(50815, format='mjd', scale='utc')
  //   >>> [a.value for a in iers_b.pm_xy(t.jd1, t.jd2)]
  //   [0.099996, 0.173811]
  EXPECT_THAT(earth_orientation_data.LookupPolarMotionArcsecInUTCScale(
                  ModifiedJulianDate(50815.0)),
              Pointwise(DoubleNear(1e-12), {0.099996, 0.173811}));

  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> t = Time(59495, format='mjd', scale='utc')
  //   >>> [a.value for a in iers_b.pm_xy(t)]
  //   [0.208782, 0.27039]
  EXPECT_THAT(earth_orientation_data.LookupPolarMotionArcsecInUTCScale(
                  ModifiedJulianDate(59495.0)),
              Pointwise(DoubleNear(1e-12), {0.208782, 0.27039}));
}

}  // namespace astro_core
