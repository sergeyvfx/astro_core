// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/body/moon.h"

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

TEST(body, GetMeeusMoonCoordinate) {
  // [Meeus1998] Page 342, Example 47.a.s
  //
  // NOTE: There are some differences from the values given in the answer in the
  // example. Partially it is caused by using different polynomial coefficient
  // for the L_prime (in this implementation coefficients from [Simon1994] are
  // used). However, even after aligning those coefficients with the book there
  // is significant difference of around 1e-5. This needs investigation.
  //
  // The values which re used to be compared about come from implementation of
  // own algorithm, and is acting more like for a regression check.
  {
    // 1992 April 12, at Oh TD
    // Astropy version 5.3.4.
    //
    // >>> from astropy.time import Time
    // >>> from astropy.utils import iers
    // >>> iers.conf.auto_download = False
    // >>> t = Time("1992-04-12T00:00:00.000", scale="utc", format="isot")
    // >>> t.tt.jd1, t.tt.jd2
    // (2448725.0, -0.4993265740740741)

    const Time time{JulianDate(2448725.0, -0.4993265740740741), TimeScale::kTT};
    const MeeusMoonCoordinate coordinate = GetMeeusMoonCoordinate(time);

    EXPECT_NEAR(coordinate.lambda,
                DegreesToRadians(133.172479872324998951 - 360.0),
                1e-12);
    EXPECT_NEAR(
        coordinate.beta, DegreesToRadians(-3.229779729011497569), 1e-12);
    EXPECT_NEAR(coordinate.delta, 368409011.413765132427215576, 1e-12);
  }

  // Validation against ERFA.
  //
  // NOTE: the ERFA's moon98 returns GCRS coordinate. The values to be compared
  // against here are obtained by adding debug prints in the ERFA's moon98.c.
  {
    const MeeusMoonCoordinate coordinate =
        GetMeeusMoonCoordinate(Time(JulianDate(2448724.5, 0), TimeScale::kTT));

    EXPECT_NEAR(coordinate.lambda, -3.959054935320557078, 1e-12);
    EXPECT_NEAR(coordinate.beta, -0.056358887879550019, 1e-12);
    EXPECT_NEAR(coordinate.delta, 368409684.816126942634582520, 1e-12);
  }
}

TEST(body, GetMoonCoordinate) {
  const Time time{JulianDate(2448725.0, -0.4993265740740741), TimeScale::kTT};
  const GCRF gcrf = GetMoonCoordinate(time);

  EXPECT_EQ(gcrf.observation_time, time);

  // The reference value is obtained using eraMoon98() and multiplying the
  // result position by ERFA_DAU.
  // See source/validate/body/moon.c.

  // TODO(sergey): The following Python code intuitively is expected to provide
  // the same coordinate, but it is quite off. Is it because the algorithm in
  // here and in ERFA does not consider epsa of Fukushima-Williams precession
  // angles?
  //
  // >>> import astropy.coordinates
  // >>> from astropy.time import Time
  // >>> from astropy.utils import iers
  // >>> from astropy import units as u
  // >>> iers.conf.auto_download = False
  // >>> t = Time("1992-04-12T00:00:00.000", scale="utc", format="isot")
  // >>> gcrs = astropy.coordinates.get_body("moon", time=t)
  // >>> (gcrs.cartesian.x.to(u.m).value, gcrs.cartesian.y.to(u.m).value,
  // ...  gcrs.cartesian.z.to(u.m).value)

  // The ERFA uses 1e-11 tolerance for the result of eraMoon98(). Here the unit
  // is in meters, which leaves less decimals which can be guaranteed.
  constexpr double kEpsilon = 1e-4;

  EXPECT_THAT(Vec3(gcrf.position.cartesian()),
              Pointwise(DoubleNear(kEpsilon),
                        {-252168459.342350006104,
                         253937183.605159878731,
                         87476708.822706788778}));

  // TODO(sergey): Support velocity calculation.
  EXPECT_FALSE(gcrf.velocity.has_value());
}

}  // namespace astro_core
