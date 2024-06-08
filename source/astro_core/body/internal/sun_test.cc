// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/body/sun.h"

#include "astro_core/coordinate/spherical.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/scale.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(body, GetApproximateSunCoordinate) {
  // Use the maximum error guaranteed by the algorithm as an epsilon.
  const double kEps = ArcsecToRadians(60.0);

  // 1992 April 12, at Oh TD
  // Astropy version 5.3.4.
  //
  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> iers.conf.auto_download = False
  // >>> t = Time("1992-04-12T00:00:00.000", scale="utc", format="isot")
  // >>> t.tt.jd1, t.tt.jd2
  // (2448725.0, -0.4993265740740741)

  // >>> import astropy.coordinates
  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> from astropy import units as u
  // >>> iers.conf.auto_download = False
  // >>> t = Time("1992-04-12T00:00:00.000", scale="utc", format="isot")
  // >>> gcrs = astropy.coordinates.get_sun(time=t)
  // >>> (gcrs.ra.to(u.rad).value, gcrs.dec.to(u.rad).value,
  // ...  gcrs.distance.to(u.m).value)
  // (0.36225156505178274, 0.1524561143930968, 149971545699.90295)

  const Time time{JulianDate(2448725.0, -0.4993265740740741), TimeScale::kTT};
  const GCRF gsrf = GetApproximateSunCoordinate(time);

  EXPECT_NEAR(
      gsrf.position.GetSpherical().longitude, 0.36225156505178274, kEps);
  EXPECT_NEAR(gsrf.position.GetSpherical().latitude, 0.1524561143930968, kEps);

  // The distance from Earth to Sun, in meters.
  // The epsilon looks quite large (is more than 10km), but assuming the
  // diameter of the Sun is 1,391,400 km the uncertainty is only about
  // 0.0008%.
  EXPECT_NEAR(gsrf.position.GetSpherical().distance, 149971545699.90295, 11000);
}

}  // namespace astro_core
