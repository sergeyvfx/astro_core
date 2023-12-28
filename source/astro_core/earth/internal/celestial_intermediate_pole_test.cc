// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/celestial_intermediate_pole.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

// The test data is verified against the ERFA C library using
// celestial_intermediate_coordinate.c to ensure there is no unwanted precision
// loss in nether of the astro_core library nor the erfa Python library.
// All implementations agreed in the values with good precision.
//
// The ERFA library uses different method for converting Arcsec to Degrees (it
// used multiplication by a pre-defined constant), which does slightly different
// result. So even though formulas are matched the result is not bit-perfect.

TEST(earth, CelestialIntermediatePole) {
  // >>> import erfa
  // >>> erfa.xy06(2459802.0, 0.4174674074074074)
  // (0.0021752506823532413, 2.4535606125214125e-05)
  EXPECT_THAT(
      CelestialIntermediatePole(JulianDate(2459802.0, 0.4174674074074074)),
      Pointwise(DoubleNear(1e-18),
                {0.0021752506823532413, 2.4535606125214125e-05}));
}

TEST(earth, CelestialIntermediateOriginLocator) {
  // >>> import erfa
  // >>> erfa.s06(2459802.0, 0.4174674074074074,
  // ...          0.0021752506823532413, 2.4535606125214125e-05)
  // (-3.575303203556072e-08)

  EXPECT_NEAR(CelestialIntermediateOriginLocator(
                  JulianDate(2459802.0, 0.4174674074074074),
                  Vec2(0.0021752506823532413, 2.4535606125214125e-05)),
              -3.575303203556072e-08,
              1e-18);
}

TEST(earth, CelestialToIntermediateFrameOfDateMatrix) {
  // >>> import numpy
  // >>> numpy.set_printoptions(precision=24)
  // >>> import erfa
  // >>> erfa.c2ixys(0.0021752506823532413, 2.4535606125214125e-05,
  // ...             -3.575303203556072e-08)
  // array([[ 9.9999763413943377e-01,  9.0674534703599763e-09,
  //         -2.1752506832304623e-03],
  //        [-6.2438526005764716e-08,  9.9999999969900200e-01,
  //         -2.4535528353406776e-05],
  //        [ 2.1752506823532413e-03,  2.4535606125214125e-05,
  //          9.9999763383843709e-01]])

  EXPECT_THAT(CelestialToIntermediateFrameOfDateMatrix(
                  Vec2(0.0021752506823532413, 2.4535606125214125e-05),
                  -3.575303203556072e-08),
              Pointwise(DoubleNear(1e-12),
                        Mat3::FromRows({{9.9999763413943377e-01,
                                         9.0674534703599763e-09,
                                         -2.1752506832304623e-03},
                                        {-6.2438526005764716e-08,
                                         9.9999999969900200e-01,
                                         -2.4535528353406776e-05},
                                        {2.1752506823532413e-03,
                                         2.4535606125214125e-05,
                                         9.9999763383843709e-01}})));

  EXPECT_THAT(CelestialToIntermediateFrameOfDateMatrix(
                  JulianDate(2459802.0, 0.4174674074074074)),
              Pointwise(DoubleNear(1e-12),
                        Mat3::FromRows({{9.9999763413943377e-01,
                                         9.0674534703599763e-09,
                                         -2.1752506832304623e-03},
                                        {-6.2438526005764716e-08,
                                         9.9999999969900200e-01,
                                         -2.4535528353406776e-05},
                                        {2.1752506823532413e-03,
                                         2.4535606125214125e-05,
                                         9.9999763383843709e-01}})));
}

}  // namespace astro_core
