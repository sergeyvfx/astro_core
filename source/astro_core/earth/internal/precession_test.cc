// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/precession.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(earth, CalculatePrecessionAngles06) {
  // >>> from astropy.time import Time
  // >>> time = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  // >>> t = (time.tt.jd1 - 2451545 + time.tt.jd2)/36525
  // >>> time.tt.jd1, time.tt.jd2
  // (2459496.0, -0.4991992592592593)

  const PrecessionAngles06 precession_angles =
      CalculatePrecessionAngles06(JulianDate(2459496.0, -0.4991992592592593));

  // >>> import erfa
  // >>> erfa.pfw06(2459496.0, -0.4991992592592593)
  // (1.0996904581340823e-05, 0.4090432454030915, 0.0053173040089238835,
  //  0.40904317345334124)
  EXPECT_NEAR(precession_angles.gamb, 1.0996904581340823e-05, 1e-16);
  EXPECT_NEAR(precession_angles.phib, 0.4090432454030915, 1e-16);
  EXPECT_NEAR(precession_angles.psib, 0.0053173040089238835, 1e-16);
  EXPECT_NEAR(precession_angles.epsa, 0.40904317345334124, 1e-16);
}

TEST(earth, PrecessionRotation) {
  // >>> import numpy as np
  // >>> import erfa
  // >>> np.set_printoptions(precision=18, suppress=False)
  // >>> r = erfa.fw2m(1.0996904581340823e-05, 0.4090432454030915,
  // ...               0.0053173040089238835, 0.40904317345334124)
  // >>> np.array(r)
  // array([[ 9.9998591676152193e-01, -4.8676161238115434e-03,
  //         -2.1148503232449566e-03],
  //        [ 4.8676162513819794e-03,  9.9998815307290179e-01,
  //         -5.0868548811412839e-06],
  //        [ 2.1148500296241921e-03, -5.2074965608934698e-06,
  //          9.9999776368861648e-01]])
  const PrecessionAngles06 precession_angles = {
      .gamb = 1.0996904581340823e-05,
      .phib = 0.4090432454030915,
      .psib = 0.0053173040089238835,
      .epsa = 0.40904317345334124,
  };
  EXPECT_THAT(PrecessionRotation(precession_angles),
              Pointwise(DoubleNear(1e-14),
                        Mat3::FromRows({{9.9998591676152193e-01,
                                         -4.8676161238115434e-03,
                                         -2.1148503232449566e-03},
                                        {4.8676162513819794e-03,
                                         9.9998815307290179e-01,
                                         -5.0868548811412839e-06},
                                        {2.1148500296241921e-03,
                                         -5.2074965608934698e-06,
                                         9.9999776368861648e-01}})));
}

}  // namespace astro_core
