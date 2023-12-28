// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second.h"

#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

// NOTE: Only basic set of tests. The actual complicated logic is tested in
// LeapSecondData.

class LeapSecondTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

TEST_F(LeapSecondTest, GetTAIMinusUTCSecondsInUTCScale) {
  // MJD is at the first row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(GetTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41317.0)), 10);
}

TEST_F(LeapSecondTest, GetTAIMinusUTCSecondsInTAIScale) {
  // Reverse of the tests in the GetTAIMinusUTCSecondsInUTCScale_Historical.
  // Basically, verify that that:
  //   GetTAIMinusUTCSecondsInTAIScale(time + leap_second) =
  //       GetTAIMinusUTCSecondsInUTCScale(time)

  // MJD is at the first row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(GetTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                41317.0, 10.0 / constants::kNumSecondsInDay)),
            10);
}

}  // namespace astro_core
