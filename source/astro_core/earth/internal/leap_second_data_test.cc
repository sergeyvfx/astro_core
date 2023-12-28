// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second_data.h"

#include "astro_core/base/constants.h"
#include "astro_core/earth/internal/leap_second_test_data.h"
#include "astro_core/earth/leap_second_table.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

namespace {

auto MakeLeapSecondData() -> LeapSecondData {
  LeapSecondData leap_second_data;

  leap_second_data.SetTable(test_data::CreateLeapSecondTable());

  return leap_second_data;
}

}  // namespace

TEST(LeapSecondData, LookupTAIMinusUTCSecondsInUTCScale_Historical) {
  const LeapSecondData leap_second_data = MakeLeapSecondData();

  // NOTE: Use the leap seconds calculated by Astropy as reference. They have
  // more decimal digits, so limit the epsilon instead.

  // TODO(sergey): Figure out where the difference is coming from.

  // Date at the beginning of the historical table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(37300.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   1.4228180050849915
  EXPECT_NEAR(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(37300.0)),
              1.4228180050849915,
              1e-7);

  // Date inbetween of 2 rows of the historical table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(37400.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   1.5524179935455322
  EXPECT_NEAR(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(37400.0)),
              1.5524179935455322,
              1e-7);
}

TEST(LeapSecondData, LookupTAIMinusUTCSecondsInUTCScale_IERS) {
  const LeapSecondData leap_second_data = MakeLeapSecondData();

  // MJD is at the first row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                ModifiedJulianDate(41317.0)),
            10);

  // MJD is at the row in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41683.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  // .  12.0
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                ModifiedJulianDate(41683.0)),
            12);

  // MJD is between two rows in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42050.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   13.0
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                ModifiedJulianDate(42050.0)),
            13);

  // MJD past the last row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42415.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   14.0
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                ModifiedJulianDate(42415.0)),
            14);

  // Lookup leap second value at the day before a new second is introduced.
  // This is a special case because the leap second gets uniformly spread over
  // the last day.

  //   >>> from astropy.time import Time
  //   >>> t = Time([41498.0, 41498.2, 41498.8, 41499.0], format='mjd',
  //   ...          scale='utc')
  //   >>> t.unix_tai - t.unix
  //   array([10. , 10.2, 10.8, 11. ])

  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                ModifiedJulianDate(41498.0)),
            10);
  EXPECT_NEAR(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(41498, 0.2)),
              10.2,
              1e-12);
  EXPECT_NEAR(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                  ModifiedJulianDate(41498, 0.8)),
              10.8,
              1e-12);
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(
                ModifiedJulianDate(41499.0)),
            11);
}

TEST(LeapSecondData, LookupTAIMinusUTCSecondsInTAIScale_Historical) {
  // Reverse of the tests in the LookupTAIMinusUTCSecondsInUTCScale_Historical.
  // Basically, verify that that:
  //   LookupTAIMinusUTCSecondsInTAIScale(time + leap_second) =
  //       LookupTAIMinusUTCSecondsInUTCScale(time)

  // THe reference value os obtained via Astropy, which has more digits in the
  // precision, so lover tolerance here. The same happens in the
  // LookupTAIMinusUTCSecondsInUTCScale_Historical tests.

  const LeapSecondData leap_second_data = MakeLeapSecondData();

  EXPECT_NEAR(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
          37300.0, 1.4228180050849915 / constants::kNumSecondsInDay)),
      1.4228180050849915,
      1e-7);

  EXPECT_NEAR(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
          37400.0, 1.5524179935455322 / constants::kNumSecondsInDay)),
      1.5524179935455322,
      1e-7);
}

TEST(LeapSecondData, LookupTAIMinusUTCSecondsInTAIScale_IERS) {
  // Reverse of the tests in the LookupTAIMinusUTCSecondsInUTCScale_Historical.
  // Basically, verify that that:
  //   LookupTAIMinusUTCSecondsInTAIScale(time + leap_second) =
  //       LookupTAIMinusUTCSecondsInUTCScale(time)

  const LeapSecondData leap_second_data = MakeLeapSecondData();

  // MJD is at the first row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
          ModifiedJulianDate(41317.0, 10.0 / constants::kNumSecondsInDay)),
      10);

  // MJD is at the row in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41683.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  // .  12.0
  EXPECT_EQ(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
          ModifiedJulianDate(41683.0, 12.0 / constants::kNumSecondsInDay)),
      12);

  // MJD is between two rows in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42050.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   13.0
  EXPECT_EQ(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
          ModifiedJulianDate(42050.0, 13.0 / constants::kNumSecondsInDay)),
      13);

  // MJD past the last row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42415.0, format='mjd')
  //   >>> t.unix_tai - t.unix
  //   14.0
  EXPECT_EQ(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
          ModifiedJulianDate(42415.0, 14.0 / constants::kNumSecondsInDay)),
      14);

  // Lookup leap second value at the day before a new second is introduced.
  // This is a special case because the leap second gets uniformly spread over
  // the last day.

  // MJD prior the day of the leap second.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41498.0, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
                ModifiedJulianDate(41498.0)),
            10);

  // Corresponding dates to LookupTAIMinusUTCSecondsInUTCScale.
  // Convert UTC values to TAI in the input data and verify the result matches
  // the number of leap seconds.
  // This is effectively a round-robin test.
  EXPECT_NEAR(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
          ModifiedJulianDate(DoubleDouble(41498, 0.2) +
                             DoubleDouble(10.2) / constants::kNumSecondsInDay)),
      10.2,
      1e-12);
  EXPECT_NEAR(
      leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
          ModifiedJulianDate(DoubleDouble(41498, 0.8) +
                             DoubleDouble(10.8) / constants::kNumSecondsInDay)),
      10.8,
      1e-12);

  // MJD at the leap second.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41499.000127314815472346, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   11.0
  EXPECT_EQ(leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(
                ModifiedJulianDate(41499.0, 0.000127314815472346)),
            11);
}

}  // namespace astro_core
