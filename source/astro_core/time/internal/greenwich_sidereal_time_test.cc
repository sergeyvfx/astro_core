// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/greenwich_sidereal_time.h"

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

TEST(time, GreenwichMeanSiderealTime1982) {
  //   >>> from astropy.time import Time
  //   >>> import erfa
  //   >>> t = Time('2006-01-15T21:24:37.500', format='isot', scale='utc')
  //   >>> erfa.gmst82(t.ut1.jd1, t.ut1.jd2)
  //   1.3324337193866924
  //   >>> t.ut1.jd1, t.ut1.jd2
  //   (2453751.0, 0.3921045609879955)
  EXPECT_THAT(
      GreenwichMeanSiderealTime1982(JulianDate(2453751.0, 0.3921045609879955)),
      NearUsingAbsDifferenceMetric(DoubleDouble(1.0, 0.3324337193866924),
                                   1e-12));

  //   >>> from astropy.time import Time
  //   >>> import erfa
  //   >>> t = Time('2022-08-10T22:00:00.000', format='isot', scale='utc')
  //   >>> erfa.gmst82(t.ut1.jd1, t.ut1.jd2)
  //   5.050310955033424
  //   >>> t.ut1.jd1, t.ut1.jd2
  //   (2459802.0, 0.41666632017409333)
  EXPECT_THAT(
      GreenwichMeanSiderealTime1982(JulianDate(2459802.0, 0.41666632017409333)),
      NearUsingAbsDifferenceMetric(DoubleDouble(5.0, 0.050310955033424),
                                   1e-12));
}

}  // namespace astro_core
