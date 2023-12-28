// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second_iers.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(LeapSecondIERS, Parse) {
  const LeapSecondIERS::Result result = LeapSecondIERS::Parse(
      "# Comment\n"
      "\n"
      "    44786.0    1  7 1981       20\n"
      "    43509.0    1  1 1978       17");

  EXPECT_TRUE(result.Ok());

  const LeapSecondTable& table = result.GetValue();

  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(43509.0)),
      17);
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(44786.0)),
      20);
}

}  // namespace astro_core
