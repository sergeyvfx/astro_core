// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation.h"

#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

// NOTE: Only basic set of tests. The actual complicated logic is tested in
// EarthOrientationData.

class EarthOrientationTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

TEST_F(EarthOrientationTest, GetUT1MinusUTCSecondsInUTCScale) {
  // The reference values has been obtained using Astropy 5.3.4.

  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> iers.conf.auto_download = False
  // >>> Time(50814, format='mjd').get_delta_ut1_utc().value
  // 0.2181283
  EXPECT_NEAR(GetUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(50814.0)),
              0.2181283,
              1e-12);
}

TEST_F(EarthOrientationTest, GetEarthPolarMotionInUTCScale) {
  // The reference values has been obtained using Astropy 5.3.4.

  //   >>> from astropy.time import Time
  //   >>> from astropy import units as u
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> t = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  //   >>> t.utc.mjd
  //   59495.0
  //   >>> [a.to_value(u.radian) for a in iers_b.pm_xy(t.jd1, t.jd2)]
  //   [1.0122036996941113e-06, 1.3108877123520745e-06]
  EXPECT_THAT(GetEarthPolarMotionInUTCScale(ModifiedJulianDate(59495.0)),
              Pointwise(DoubleNear(1e-12),
                        {1.0122036996941113e-06, 1.3108877123520745e-06}));
}

}  // namespace astro_core
