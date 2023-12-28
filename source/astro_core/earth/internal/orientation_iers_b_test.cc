// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation_iers_b.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(EarthOrientationIERSB, Parse) {
  // clang-format off
  const EarthOrientationIERSB::Result result = EarthOrientationIERSB::Parse(
    "             FORMAT(3(I4),I7,2(F11.6),2(F12.7),2(F11.6),2(F11.6),2(F11.7),2(F12.6))\n"
    "##################################################################################\n"
    "  \n"
    "      Date      MJD      x          y        UT1-UTC       LOD         dX        dY        x Err     y Err   UT1-UTC Err  LOD Err     dX Err       dY Err  \n"
    "                         "          "           s           s          "         "           "          "          s         s            "           "\n"
    "     (0h UTC)\n"
    "\n"
    "1962   1   1  37665  -0.012700   0.213000   0.0326338   0.0017230   0.000000   0.000000   0.030000   0.030000  0.0020000  0.0014000    0.004774    0.002000\n"
    "1962   1   2  37666  -0.015900   0.214100   0.0320547   0.0016690   0.000000   0.000000   0.030000   0.030000  0.0020000  0.0014000    0.004774    0.002000\n"
    "1962   1   3  37667  -0.019000   0.215200   0.0315526   0.0015820   0.000000   0.000000   0.030000   0.030000  0.0020000  0.0014000    0.004774    0.002000\n"
    "1962   1   4  37668  -0.021999   0.216301   0.0311435   0.0014960   0.000000   0.000000   0.030000   0.030000  0.0020000  0.0014000    0.004774    0.002000\n"
    "1962   1   5  37669  -0.024799   0.217301   0.0308154   0.0014160   0.000000   0.000000   0.030000   0.030000  0.0020000  0.0014000    0.004774    0.002000\n"
  );
  // clang-format on

  EXPECT_TRUE(result.Ok());

  const EarthOrientationTable& table = result.GetValue();

  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(37665.0)),
      0.0326338,
      1e-12);
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(37666.0)),
      0.0320547,
      1e-12);
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(37667.0)),
      0.0315526,
      1e-12);
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(37668.0)),
      0.0311435,
      1e-12);
}

}  // namespace astro_core
