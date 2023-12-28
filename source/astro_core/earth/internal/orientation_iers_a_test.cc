// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation_iers_a.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(IERS_A, Parse) {
  // clang-format off
  const EarthOrientationIERSA::Result result = EarthOrientationIERSA::Parse(
    "73 1 2 41684.00 I  0.120733 0.009786  0.136966 0.015902  I 0.8084178 0.0002710  0.0000 0.1916  P    -0.766    0.199    -0.720    0.300   .143000   .137000   .8075000   -18.637    -3.667  \n"
    " 4 2 9 53044.00 I -0.062593 0.000057  0.195562 0.000080  I-0.4052923 0.0000059  0.7558 0.0037  I    -0.057    0.076    -0.171    0.140 -0.062540  0.195330 -0.4052940    -0.083    -0.212  \n"
    "221111 59894.00 P  0.188039 0.000625  0.195858 0.000400  P-0.0163763 0.0001080                 P     0.073    0.128    -0.077    0.160                                                     \n"
    "23 126 59970.00 P  0.021608 0.007439  0.266497 0.009130  P-0.0176126 0.0074544                                                                                                             \n"
    "231119 60267.00                                                                                                                                                                            \n"
  );
  // clang-format on

  EXPECT_TRUE(result.Ok());

  const EarthOrientationTable& table = result.GetValue();

  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(41684.0)),
      0.8075000,
      1e-12);
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(53044.0)),
      -0.4052940,
      1e-12);
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(59894.0)),
      -0.0163763,
      1e-12);
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(59970.0)),
      -0.0176126,
      1e-12);
}

}  // namespace astro_core
