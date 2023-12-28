// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/time.h"

#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/time/format/unix_time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

TEST(Time, AsScalar) {
  EXPECT_EQ(Time(UnixTime(0), TimeScale::kUTC).AsFormat<DateTime>(),
            DateTime(1970, 1, 1));

  EXPECT_EQ(Time(UnixTime(1667662059.0), TimeScale::kUTC).AsFormat<DateTime>(),
            DateTime(2022, 11, 5, 15, 27, 39));
}

TEST(Time, Precision) {
  EXPECT_THAT(
      Time(UnixTime(1667662059.0), TimeScale::kUTC).AsFormat<UnixTime>(),
      NearUsingAbsDifferenceMetric(DoubleDouble(1667662059.0), 1e-12));
}

TEST(Time, UnaryAddition) {
  {
    Time time = Time(UnixTime(1000), TimeScale::kUTC);
    time += TimeDifference::FromSeconds(200);
    EXPECT_THAT(time.AsFormat<UnixTime>(),
                NearUsingAbsDifferenceMetric(DoubleDouble(1200.0), 1e-12));
  }
}

TEST(Time, UnaryDifference) {
  {
    Time time = Time(UnixTime(1000), TimeScale::kUTC);
    time -= TimeDifference::FromSeconds(200);
    EXPECT_THAT(time.AsFormat<UnixTime>(),
                NearUsingAbsDifferenceMetric(DoubleDouble(800.0), 1e-12));
  }
}

TEST(Time, BinaryAddition) {
  {
    EXPECT_THAT((Time(UnixTime(1000), TimeScale::kUTC) +
                 TimeDifference::FromSeconds(200))
                    .AsFormat<UnixTime>(),
                NearUsingAbsDifferenceMetric(DoubleDouble(1200.0), 1e-12));
  }
}

TEST(Time, BinaryDifference) {
  {
    EXPECT_THAT((Time(UnixTime(1000), TimeScale::kUTC) -
                 TimeDifference::FromSeconds(200))
                    .AsFormat<UnixTime>(),
                NearUsingAbsDifferenceMetric(DoubleDouble(800.0), 1e-12));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Time scale conversion tests.
//
// General notes:
//
//  - The scale conversion is verified against Astropy.
//
//  - The inputs are usually in the modified Julian dates (MJD), but the
//    output is in Julian dates. this is because Astropy provides a way to
//    get more precision out of the Julian date using jd1 and jd2.

class TimeScaleTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

// No-op conversion.

TEST_F(TimeScaleTest, ToScale_Simple) {
  // TAI to TAI.
  EXPECT_EQ(Time(ModifiedJulianDate(57204.0), TimeScale::kTAI)
                .ToScale<TimeScale::kTAI>()
                .GetScale(),
            TimeScale::kTAI);
  EXPECT_THAT(Time(ModifiedJulianDate(57204.0), TimeScale::kTAI)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(57204.0), 1e-12));

  // UTC to UTC.
  EXPECT_EQ(Time(ModifiedJulianDate(57204.0), TimeScale::kUTC)
                .ToScale<TimeScale::kUTC>()
                .GetScale(),
            TimeScale::kUTC);
  EXPECT_THAT(Time(ModifiedJulianDate(57204.0), TimeScale::kUTC)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(57204.0), 1e-12));

  // UT1 to UT1.
  EXPECT_EQ(Time(ModifiedJulianDate(57204.0), TimeScale::kUT1)
                .ToScale<TimeScale::kUT1>()
                .GetScale(),
            TimeScale::kUT1);
  EXPECT_THAT(Time(ModifiedJulianDate(57204.0), TimeScale::kUT1)
                  .ToScale<TimeScale::kUT1>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(57204.0), 1e-12));

  // TT(TAI) to TT(TAI).
  EXPECT_EQ(Time(ModifiedJulianDate(57204.0), TimeScale::kTT)
                .ToScale<TimeScale::kTT>()
                .GetScale(),
            TimeScale::kTT);
  EXPECT_THAT(Time(ModifiedJulianDate(57204.0), TimeScale::kTT)
                  .ToScale<TimeScale::kTT>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(57204.0), 1e-12));
}

// UTC time scale.

TEST_F(TimeScaleTest, ToScale_UTC_To_TAI) {
  EXPECT_EQ(Time(ModifiedJulianDate(41317.0), TimeScale::kUTC)
                .ToScale<TimeScale::kTAI>()
                .GetScale(),
            TimeScale::kTAI);

  // Time at the very beginning of the leap second table.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41317.0, format='mjd', scale='utc')
  //   >>> t.tai.jd1, t.tai.jd2
  //   (2441318.0, -0.4998842592592593)
  EXPECT_THAT(Time(ModifiedJulianDate(41317.0), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2441318.0, -0.4998842592592593), 1e-12));

  // Date prior to the leap second which falls under the historical table from
  // HPIERS.
  //
  // The TAI-UTC is calculated the HPIERS historical table as:
  //
  //   >>> import numpy
  //   >>> numpy.float128(4.213170) + (41316.8 - 39126) * 0.002592
  //   9.891723600000007721
  //
  // The TAI time is calculated as
  //
  //   >>> numpy.float128(41316.8) + 9.891723600000007721/86400.0 + 2400000.5
  //   2441317.3001144875445
  //
  // The similar calculation using Astropy will be:
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(41316.8, format='mjd', scale='utc')
  //   >>> t.tai.jd1, t.tai.jd2
  //   (2441317.0, 0.3001154853038662)
  //
  // The Astropy uses different approach calculating the TAI-UTC for the
  // historical dates, so its result is not directly usable.
  //
  // TODO(sergey): Figure out whether this difference is acceptable.
  EXPECT_THAT(Time(ModifiedJulianDate(41316.8), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2441317.0, 0.3001144875445), 1e-12));

  // Time in the middle of the table, between of 2 rows.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(42048.3, format='mjd', scale='utc')
  //   >>> t.tai.jd1, t.tai.jd2
  //   (2442049.0, -0.19984953703412667)
  EXPECT_THAT(Time(ModifiedJulianDate(42048.3), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2442049.0, -0.19984953703412667), 1e-12));

  // Lookup leap second value at the day before a new second is introduced.
  // This is a special case because the leap second gets uniformly spread over
  // the last day.

  // Time in the middle of the table, between of 2 rows.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time([41498.0, 41498.2, 41498.8, 41499.0], format='mjd',
  //   ...          scale='utc')
  //   >>> [x for x in zip(t.tai.jd1, t.tai.jd2)]
  //   [(2441499.0, -0.4998842592592593),
  //    (2441499.0, -0.29988194444735483),
  //    (2441499.0, 0.3001250000029104),
  //    (2441500.0, -0.4998726851851852)]

  EXPECT_THAT(Time(ModifiedJulianDate(41498.0), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2441499.0, -0.4998842592592593), 1e-12));
  EXPECT_THAT(Time(ModifiedJulianDate(41498.2), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2441499.0, -0.29988194444735483), 1e-12));
  EXPECT_THAT(Time(ModifiedJulianDate(41498.8), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2441499.0, 0.3001250000029104), 1e-12));
  EXPECT_THAT(Time(ModifiedJulianDate(41499.0), TimeScale::kUTC)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2441500.0, -0.4998726851851852), 1e-12));
}

TEST_F(TimeScaleTest, ToScale_TAI_To_UTC) {
  EXPECT_EQ(Time(ModifiedJulianDate(41320.0), TimeScale::kTAI)
                .ToScale<TimeScale::kUTC>()
                .GetScale(),
            TimeScale::kUTC);

  // Time at the very beginning of the leap second table.
  // The reverse of the same test in ToScale_UTC_To_TAI.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(2441318.0, -0.4998842592592593, format='jd', scale='tai')
  //   >>> t.utc.jd1, t.utc.jd2
  //   (2441318.0, -0.5)
  //   >>> t.utc.mjd
  //   41317.0
  EXPECT_THAT(Time(JulianDate(2441318.0, -0.4998842592592593), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(41317.0), 1e-12));

  // Date prior to the leap second which falls under the historical table
  // from HPIERS. The reverse of the same test in ToScale_UTC_To_TAI.
  EXPECT_THAT(Time(JulianDate(2441317.0, 0.3001144875445), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(41316.8), 1e-12));

  // Time in the middle of the table, between of 2 rows.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(2442049.0, -0.19984953703412667, format='jd', scale='tai')
  //   >>> t.utc.jd1, t.utc.jd2
  //   (2442049.0, -0.19999999999708962)
  //   >>> t.utc.mjd
  //   42048.3
  EXPECT_THAT(Time(JulianDate(2442049.0, -0.19984953703412667), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(42048.3), 1e-12));

  // Lookup leap second value at the day before a new second is introduced.
  // This is a special case because the leap second gets uniformly spread over
  // the last day.
  //
  // The tests are the reverse of UTC-to-TAI conversion.

  EXPECT_THAT(Time(JulianDate(2441499.0, -0.4998842592592593), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(41498.0), 1e-12));
  EXPECT_THAT(Time(JulianDate(2441499.0, -0.29988194444735483), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(41498.2), 1e-12));
  EXPECT_THAT(Time(JulianDate(2441499.0, 0.3001250000029104), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(41498.8), 1e-12));
  EXPECT_THAT(Time(JulianDate(2441500.0, -0.4998726851851852), TimeScale::kTAI)
                  .ToScale<TimeScale::kUTC>()
                  .AsFormat<ModifiedJulianDate>(),
              NearUsingAbsDifferenceMetric(ModifiedJulianDate(41499.0), 1e-12));
}

TEST_F(TimeScaleTest, ToScale_UTC_To_UT1) {
  // Time used in other tests.
  // This is 2022-08-10T22:00:00.000 ISOT.

  // The reference values has been obtained using Astropy 5.3.4.

  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(2022, 8, 10, 22, 0, 0), scale='utc')
  //   >>> t.jd1, t.jd2
  //   (2459802.0, 0.41666666666666663)
  //   >>> t.tai.jd1, t.tai.jd2
  //   (2459802.0, 0.4170949074074074)
  //   >>> t.ut1.jd1, t.ut1.jd2
  //   (2459802.0, 0.4166663200011574)
  {
    const Time time = Time(DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC);
    // Test that the DateTime is properly converted to JD.
    EXPECT_THAT(time.AsFormat<JulianDate>(),
                NearUsingAbsDifferenceMetric(
                    JulianDate(2459802.0, 0.41666666666666663), 1e-12));
    // Test conversion to TAI is correct.
    EXPECT_THAT(time.ToScale<TimeScale::kTAI>().AsFormat<JulianDate>(),
                NearUsingAbsDifferenceMetric(
                    JulianDate(2459802.0, 0.4170949074074074), 1e-12));
    // Test conversion to UT1 is correct.
    EXPECT_THAT(time.ToScale<TimeScale::kUT1>().AsFormat<JulianDate>(),
                NearUsingAbsDifferenceMetric(
                    JulianDate(2459802.0, 0.4166663200011574), 1e-12));
  }
}

// UT1 time scale.

TEST_F(TimeScaleTest, ToScale_UT1_To_TAI) {
  // The reference values has been obtained using Astropy 5.3.4.

  // The output is at the exact row of the Earth orientation parameters table
  // which is January 2, 1998 (converted to TAI).
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> t = Time(2450816.0, -0.4999974943206018, format='jd', scale='ut1')
  //   >>> t.tai.jd1, t.tai.jd2
  //   (2450816.0, -0.49964120431828696)
  EXPECT_THAT(Time(JulianDate(2450816.0, -0.4999974943206018), TimeScale::kUT1)
                  .ToScale<TimeScale::kTAI>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2450816.0, -0.49964120431828696), 1e-12));

  // Test of the conversion around the leap second.
  //
  // The output is around midnight July 1, 1997 (50630 MJD) converted to TAI.
  // At this a leap second has been introduced. This matches the input data for
  // the TAI-to-UT1 test.
  //
  // Conversion of the 50630 MJD and 2 points 0.5 sec away to UT1:
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> t = Time([50630.0 - 0.5/86400, 50630.0, 50630.0 + 0.5/86400],
  //   ...          format='mjd', scale='utc')
  //   >>> [x for x in zip(t.ut1.jd1, t.ut1.jd2)]
  //   [(2450631.0, -0.49999968886671586),
  //    (2450631.0, -0.4999939017650463),
  //    (2450631.0, -0.4999881147303446)]
  {
    //   >>> from astropy.time import Time
    //   >>> from astropy.utils import iers
    //   >>> iers.conf.auto_download = False
    //   >>> t = Time(2450631.0, -0.49999968886671586, format='jd', scale='ut1')
    //   >>> t.tai.jd1, t.tai.jd2
    //   (2450631.0, -0.49964699080543507)
    EXPECT_THAT(
        Time(JulianDate(2450631.0, -0.49999968886671586), TimeScale::kUT1)
            .ToScale<TimeScale::kTAI>()
            .AsFormat<JulianDate>(),
        NearUsingAbsDifferenceMetric(
            JulianDate(2450631.0, -0.49964699080543507), 1e-12));

    //   >>> from astropy.time import Time
    //   >>> from astropy.utils import iers
    //   >>> iers.conf.auto_download = False
    //   >>> t = Time(2450631.0, -0.4999939017650463, format='jd', scale='ut1')
    //   >>> t.tai.jd1, t.tai.jd2
    //   (2450631.0, -0.4996412037037037)
    EXPECT_THAT(
        Time(JulianDate(2450631.0, -0.4999939017650463), TimeScale::kUT1)
            .ToScale<TimeScale::kTAI>()
            .AsFormat<JulianDate>(),
        NearUsingAbsDifferenceMetric(JulianDate(2450631.0, -0.4996412037037037),
                                     1e-12));

    //   >>> from astropy.time import Time
    //   >>> from astropy.utils import iers
    //   >>> iers.conf.auto_download = False
    //   >>> t = Time(2450631.0, -0.4999881147303446, format='jd', scale='ut1')
    //   >>> t.tai.jd1, t.tai.jd2
    //   (2450631.0, -0.49963541666895184)
    EXPECT_THAT(
        Time(JulianDate(2450631.0, -0.4999881147303446), TimeScale::kUT1)
            .ToScale<TimeScale::kTAI>()
            .AsFormat<JulianDate>(),
        NearUsingAbsDifferenceMetric(
            JulianDate(2450631.0, -0.49963541666895184), 1e-12));
  }
}

TEST_F(TimeScaleTest, ToScale_TAI_To_UT1) {
  // The reference values has been obtained using Astropy 5.3.4.

  // Test to verify the scale is assigned.
  EXPECT_EQ(Time(ModifiedJulianDate(50815.0), TimeScale::kTAI)
                .ToScale<TimeScale::kUT1>()
                .GetScale(),
            TimeScale::kUT1);

  // The input is at the exact row of the Earth orientation parameters table
  // which is January 2, 1998 (converted to TAI).
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> t = Time(2450816.0, -0.4996412037037037, format='jd', scale='tai')
  //   >>> t.ut1.jd1, t.ut1.jd2
  //   (2450816.0, -0.4999974937060185)
  EXPECT_THAT(Time(JulianDate(2450816.0, -0.4996412037037037), TimeScale::kTAI)
                  .ToScale<TimeScale::kUT1>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2450816.0, -0.4999974937060185), 1e-12));

  // The input is between January 2 and 3, 1998.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> t = Time(2450816.2, -0.4996412037037037, format='jd', scale='tai')
  //   >>> t.ut1.jd1, t.ut1.jd2
  //   (2450816.0, -0.29999749758757804)
  EXPECT_THAT(Time(JulianDate(2450816.2, -0.4996412037037037), TimeScale::kTAI)
                  .ToScale<TimeScale::kUT1>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(
                  JulianDate(2450816.0, -0.29999749758757804), 1e-12));

  // Test of the conversion around the leap second.
  //
  // The input is around midnight July 1, 1997 (50630 MJD) converted to TAI.
  // At this a leap second has been introduced.
  //
  // Conversion of the 50630 MJD and 2 points 0.5 sec away to TAI:
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> t = Time([50630.0 - 0.5/86400, 50630.0, 50630.0 + 0.5/86400],
  //   ...          format='mjd', scale='utc')
  //   >>> [x for x in zip(t.tai.jd1, t.tai.jd2)]
  //   [(2450631.0, -0.49964699080543507),
  //    (2450631.0, -0.4996412037037037),
  //    (2450631.0, -0.49963541666895184)]
  {
    //   >>> from astropy.time import Time
    //   >>> from astropy.utils import iers
    //   >>> iers.conf.auto_download = False
    //   >>> t = Time(2450631.0, -0.49964699080543507, format='jd', scale='tai')
    //   >>> t.ut1.jd1, t.ut1.jd2
    //   (2450631.0, -0.4999996888667158)
    EXPECT_THAT(
        Time(JulianDate(2450631.0, -0.49964699080543507), TimeScale::kTAI)
            .ToScale<TimeScale::kUT1>()
            .AsFormat<JulianDate>(),
        NearUsingAbsDifferenceMetric(JulianDate(2450631.0, -0.4999996888667158),
                                     1e-12));

    //   >>> from astropy.time import Time
    //   >>> from astropy.utils import iers
    //   >>> iers.conf.auto_download = False
    //   >>> t = Time(2450631.0, -0.4996412037037037, format='jd', scale='tai')
    //   >>> t.ut1.jd1, t.ut1.jd2
    //   (2450631.0, -0.49999390176504627)
    EXPECT_THAT(
        Time(JulianDate(2450631.0, -0.4996412037037037), TimeScale::kTAI)
            .ToScale<TimeScale::kUT1>()
            .AsFormat<JulianDate>(),
        NearUsingAbsDifferenceMetric(
            JulianDate(2450631.0, -0.49999390176504627), 1e-12));

    //   >>> from astropy.time import Time
    //   >>> from astropy.utils import iers
    //   >>> iers.conf.auto_download = False
    //   >>> t = Time(2450631.0, -0.49963541666895184, format='jd', scale='tai')
    //   >>> t.ut1.jd1, t.ut1.jd2
    //   (2450631.0, -0.4999881147303446)
    EXPECT_THAT(
        Time(JulianDate(2450631.0, -0.49963541666895184), TimeScale::kTAI)
            .ToScale<TimeScale::kUT1>()
            .AsFormat<JulianDate>(),
        NearUsingAbsDifferenceMetric(JulianDate(2450631.0, -0.4999881147303446),
                                     1e-12));
  }
}

// TT(TAI) time scale.

TEST_F(TimeScaleTest, ToScale_TT_To_TAI) {
  EXPECT_EQ(Time(ModifiedJulianDate(41317.0), TimeScale::kTT)
                .ToScale<TimeScale::kTAI>()
                .GetScale(),
            TimeScale::kTAI);

  // >>> from astropy.time import Time
  // >>> t = Time(2448724.5, 0, format='jd', scale='tt')
  // >>> t.tai.jd1, t.tai.jd2
  // (2448724.0, 0.4996275)

  EXPECT_THAT(
      Time(JulianDate(2448724.5, 0), TimeScale::kTT)
          .ToScale<TimeScale::kTAI>()
          .AsFormat<JulianDate>(),
      NearUsingAbsDifferenceMetric(JulianDate(2448724.0, 0.4996275), 1e-12));
}

TEST_F(TimeScaleTest, ToScale_TAI_To_TT) {
  EXPECT_EQ(Time(ModifiedJulianDate(41317.0), TimeScale::kTAI)
                .ToScale<TimeScale::kTT>()
                .GetScale(),
            TimeScale::kTT);

  // >>> from astropy.time import Time
  // >>> t = Time(2448724.0, 0.4996275, format='jd', scale='tai')
  // >>> t.tt.jd1, t.tt.jd2
  // (2448724.0, 0.5)

  EXPECT_THAT(Time(JulianDate(2448724.0, 0.4996275), TimeScale::kTAI)
                  .ToScale<TimeScale::kTT>()
                  .AsFormat<JulianDate>(),
              NearUsingAbsDifferenceMetric(JulianDate(2448724.0, 0.5), 1e-12));
}

// Tests from the Astropy.

TEST_F(TimeScaleTest, Astropy) {
  // Tests from https://docs.astropy.org/en/stable/time/index.html#id18
  // The reference values has been obtained using Astropy 5.3.4.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> import astropy.units as u
  //   >>> t = Time('2006-01-15 21:24:37.5', format='iso', scale='utc',
  //   ...          location=(-155.933222*u.deg, 19.48125*u.deg))
  //   >>> t.utc.datetime
  //   datetime.datetime(2006, 1, 15, 21, 24, 37, 500000)
  //   >>> t.ut1.datetime
  //   datetime.datetime(2006, 1, 15, 21, 24, 37, 834110)
  //   >>> t.tai.datetime
  //   datetime.datetime(2006, 1, 15, 21, 25, 10, 500000)
  //   >>> t.tt.datetime
  //   datetime.datetime(2006, 1, 15, 21, 25, 42, 684000)
  //   >>> t.tcg.datetime
  //   datetime.datetime(2006, 1, 15, 21, 25, 43, 322690)
  //   >>> t.tdb.datetime
  //   datetime.datetime(2006, 1, 15, 21, 25, 42, 684373)
  //   >>> t.tcb.datetime
  //   datetime.datetime(2006, 1, 15, 21, 25, 56, 893952)

  const Time t =
      Time(DateTime(2006, 1, 15, 21, 24, 37, 500000), TimeScale::kUTC);

  EXPECT_EQ(t.ToScale<TimeScale::kUTC>().AsFormat<DateTime>(),
            DateTime(2006, 1, 15, 21, 24, 37, 500000));

  EXPECT_EQ(t.ToScale<TimeScale::kUT1>().AsFormat<DateTime>(),
            DateTime(2006, 1, 15, 21, 24, 37, 834110));

  EXPECT_EQ(t.ToScale<TimeScale::kTAI>().AsFormat<DateTime>(),
            DateTime(2006, 1, 15, 21, 25, 10, 500000));

  EXPECT_EQ(t.ToScale<TimeScale::kTT>().AsFormat<DateTime>(),
            DateTime(2006, 1, 15, 21, 25, 42, 684000));
}

}  // namespace astro_core
