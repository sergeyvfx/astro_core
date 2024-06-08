// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/intermediate_rotation.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(earth, BiasPrecessionNutationRotation06A) {
  // >>> from astropy.time import Time
  // >>> time = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  // >>> t = (time.tt.jd1 - 2451545 + time.tt.jd2)/36525
  // >>> time.tt.jd1, time.tt.jd2
  // (2459496.0, -0.4991992592592593)

  // >>> import numpy as np
  // >>> import erfa
  // >>> np.set_printoptions(precision=18, suppress=False)
  // >>> rbpn = erfa.pnm06a(2459496.0, -0.4991992592592593)
  // >>> np.array(rbpn)
  // array([[ 9.9998633559245698e-01, -4.7946635537646559e-03,
  //         -2.0832258101703663e-03],
  //        [ 4.7946169259878183e-03,  9.9998850538347850e-01,
  //         -2.7376080570845867e-05],
  //        [ 2.0833331233843086e-03,  1.7387436762787444e-05,
  //          9.9999782970803186e-01]])
  EXPECT_THAT(BiasPrecessionNutationRotation06A(
                  JulianDate(2459496.0, -0.4991992592592593)),
              Pointwise(DoubleNear(1e-10),
                        Mat3::FromRows({{9.9998633559245698e-01,
                                         -4.7946635537646559e-03,
                                         -2.0832258101703663e-03},
                                        {4.7946169259878183e-03,
                                         9.9998850538347850e-01,
                                         -2.7376080570845867e-05},
                                        {2.0833331233843086e-03,
                                         1.7387436762787444e-05,
                                         9.9999782970803186e-01}})));
}

}  // namespace astro_core
