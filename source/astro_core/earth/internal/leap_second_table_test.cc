// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second_table.h"

#include "astro_core/earth/internal/leap_second_test_data.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(LeapSecondTable, LookupTAIMinusUTCSecondsInUTCScale) {
  const LeapSecondTable table = test_data::CreateLeapSecondTable();

  // MJD is prior the first table row.
  // It is not possible to know the TAI-UTC value at that time, but the lookup
  // is expected to be clamped to the first row.
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41316.0)),
      10);

  // MJD is at the first row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, format='mjd', scale='utc')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41317.0)),
      10);

  // MJD is at the row in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41683.0, format='mjd', scale='utc')
  //   >>> t.unix_tai - t.unix
  //   12.0
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41683.0)),
      12);

  // MJD is between two rows in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42050.0, format='mjd', scale='utc')
  //   >>> t.unix_tai - t.unix
  //   13.0
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(42050.0)),
      13);

  // MJD past the last row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42415.0, format='mjd', scale='utc')
  //   >>> t.unix_tai - t.unix
  //   14.0
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(42415.0)),
      14);

  // Lookup leap second value at the day before a new second is introduced.
  // This is a special case because the leap second gets uniformly spread over
  // the last day.

  //   >>> from astropy.time import Time
  //   >>> t = Time([41498.0, 41498.2, 41498.8, 41499.0], format='mjd',
  //   ...          scale='utc')
  //   >>> t.unix_tai - t.unix
  //   array([10. , 10.2, 10.8, 11. ])

  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41498.0)),
      10);
  EXPECT_NEAR(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41498, 0.2)),
      10.2,
      1e-12);
  EXPECT_NEAR(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41498, 0.8)),
      10.8,
      1e-12);
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInUTCScale(ModifiedJulianDate(41499.0)),
      11);
}

TEST(LeapSecondTable, LookupTAIMinusUTCSecondsInTAIScale) {
  const LeapSecondTable table = test_data::CreateLeapSecondTable();

  // The tests here basically follow same data as in
  // LookupTAIMinusUTCSecondsInUTCScale by adding a leap second the the input
  // test data.

  // MJD is prior the first table row.
  // It is not possible to know the TAI-UTC value at that time, but the lookup
  // is expected to be clamped to the first row.
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(41316.0)),
      10);

  // MJD is at the first row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, 10.0 / 86400, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   10.0
  EXPECT_EQ(table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                41317.0, 10.0 / constants::kNumSecondsInDay)),
            10);

  // MJD is at the row in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41683.0, 12.0 / 86400, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   12.0
  EXPECT_EQ(table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                41683.0, 12.0 / constants::kNumSecondsInDay)),
            12);

  // MJD is between two rows in the middle of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42050.0, 13.0 / 86400, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   13.0
  EXPECT_EQ(table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                42050.0, 13.0 / constants::kNumSecondsInDay)),
            13);

  // MJD past the last row of the table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42415.0, 14.0 / 864000, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   14.0
  EXPECT_EQ(table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                42415.0, 14.0 / constants::kNumSecondsInDay)),
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
  EXPECT_EQ(
      table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(41498.0)),
      10);

  // Corresponding dates to LookupTAIMinusUTCSecondsInUTCScale.
  // Convert UTC values to TAI in the input data and verify the result matches
  // the number of leap seconds.
  // This is effectively a round-robin test.
  EXPECT_NEAR(table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                  DoubleDouble(41498, 0.2) +
                  DoubleDouble(10.2) / constants::kNumSecondsInDay)),
              10.2,
              1e-12);
  EXPECT_NEAR(table.LookupTAIMinusUTCSecondsInTAIScale(ModifiedJulianDate(
                  DoubleDouble(41498, 0.8) +
                  DoubleDouble(10.8) / constants::kNumSecondsInDay)),
              10.8,
              1e-12);

  // MJD at the leap second.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41499.000127314815472346, format='mjd', scale='tai')
  //   >>> t.unix_tai - t.unix
  //   11.0
  EXPECT_EQ(table.LookupTAIMinusUTCSecondsInTAIScale(
                ModifiedJulianDate(41499.0, 0.000127314815472346)),
            11);
}

}  // namespace astro_core
