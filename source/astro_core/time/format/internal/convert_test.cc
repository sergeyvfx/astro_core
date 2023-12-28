// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format//internal/convert.h"

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_format_internal {

using testing::NearUsingAbsDifferenceMetric;

TEST(Time, DateTimeToJulianDate) {
  // [Vallado2013] example 3-4.
  //
  // Verified and expanded the precision for comparision using Astropy:
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(1996, 10, 26, 14, 20, 0))
  //   >>> (t.jd1, t.jd2)
  //   (2450383.0, 0.09722222222222221)
  EXPECT_THAT(DateTimeToJulianDate(1996, 10, 26, 14, 20, 0),
              NearUsingAbsDifferenceMetric(
                  DoubleDouble(2450383, 0.0972222222222222), 1e-12));

  // [Meeus1998] page 61, example 7.b.
  //
  // Verified and expanded the precision for comparision using Astropy:
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(333, 1, 27, 12, 0, 0))
  //   >>> (t.jd1, t.jd2)
  //   (1842712.0, 0.0)
  //
  // NOTE: The example shows example for the date in Julian calendar, so the
  // value from the book is different from the one computed here and in the
  // Astropy.
  EXPECT_THAT(DateTimeToJulianDate(333, 1, 27, 12, 0, 0),
              NearUsingAbsDifferenceMetric(DoubleDouble(1842712.0), 1e-12));

  // Example from the Python's sgp4 package usage example:
  //
  //   >>> from sgp4.api import jday
  //   >>> jday(2019, 12, 9, 12, 0, 0)
  //   (2458826.5, 0.5)
  //
  // Verified using Astpropy:
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(2019, 12, 9, 12, 0, 0))
  //   >>> (t.jd1, t.jd2)
  //   (2458827.0, 0.0)
  EXPECT_THAT(DateTimeToJulianDate(2019, 12, 9, 12, 0, 0),
              NearUsingAbsDifferenceMetric(DoubleDouble(2458827.0), 1e-12));

  // Dates well outside of the 1900..2100 range.
  // Used to test against limitations of the original [Vallado2013]
  // algorithm 14.
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(1800, 1, 1, 12, 0, 0))
  //   >>> (t.jd1, t.jd2)
  //   (2378497.0, 0.0)
  //   >>> t = Time(datetime(2200, 1, 1, 12, 0, 0))
  //   >>> (t.jd1, t.jd2)
  //
  // Note that the Astropy will warn about dubious year.
  EXPECT_THAT(DateTimeToJulianDate(1800, 1, 1, 12, 0, 0),
              NearUsingAbsDifferenceMetric(DoubleDouble(2378497.0), 1e-12));
  EXPECT_THAT(DateTimeToJulianDate(2200, 1, 1, 12, 0, 0),
              NearUsingAbsDifferenceMetric(DoubleDouble(2524594.0), 1e-12));

  // Dates which showed to be problematic during development.
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(2022, 11, 5, 15, 27, 39, 0))
  //   >>> (t.jd1, t.jd2)
  //   (2459889.0, 0.1442013888888889)
  EXPECT_THAT(DateTimeToJulianDate(2022, 11, 5, 15, 27, 39),
              NearUsingAbsDifferenceMetric(
                  DoubleDouble(2459889.0, 0.1442013888888889), 1e-12));
}

TEST(Time, JulianDateToDateTime) {
  // [Meeus1998] page 64, example 7.c.
  //
  //   >>> from astropy.time import Time
  //   >>> Time(2436116.31, format='jd').datetime
  //   datetime.datetime(1957, 10, 4, 19, 26, 24, 5)
  //
  // TODO(sergey): Investigate where the 5 microseconds are coming from. Seems
  // that calculation of the similar date from MJD does not have the
  // microseconds.
  {
    int year, month, day, hour, minute;
    double seconds;
    JulianDateToDateTime(2436116.31, year, month, day, hour, minute, seconds);

    EXPECT_EQ(year, 1957);
    EXPECT_EQ(month, 10);
    EXPECT_EQ(day, 4);
    EXPECT_EQ(hour, 19);
    EXPECT_EQ(minute, 26);
    EXPECT_NEAR(seconds, 24.000004827976227, 1e-12);
  }
}

TEST(Time, YearDecimalDayToDateTime) {
  // Cross-referenced with days2mdhms_SGP4 from the SGP library.
  //
  // There are differences in handling the leap year, but it is not noticeable
  // in practice since the year 2000 is not commonly used in the typical usecase
  // of this format.
  //
  // There are also some precision differences, but they agree on a level better
  // than a nanosecond.

  {
    int year, month, day, hour, minute;
    double seconds;
    YearDecimalDayToDateTime(
        2008, 264.51782528, year, month, day, hour, minute, seconds);

    EXPECT_EQ(year, 2008);
    EXPECT_EQ(month, 9);
    EXPECT_EQ(day, 20);

    EXPECT_EQ(hour, 12);
    EXPECT_EQ(minute, 25);
    EXPECT_NEAR(seconds, 40.104192000945, 1e-10);
  }

  {
    int year, month, day, hour, minute;
    double seconds;
    YearDecimalDayToDateTime(
        2011, 128.751, year, month, day, hour, minute, seconds);

    EXPECT_EQ(year, 2011);
    EXPECT_EQ(month, 5);
    EXPECT_EQ(day, 8);

    EXPECT_EQ(hour, 18);
    EXPECT_EQ(minute, 1);
    EXPECT_NEAR(seconds, 26.400000000413, 1e-10);
  }
}

TEST(Time, DateTimeToDecimalDay) {
  EXPECT_NEAR(DateTimeToDecimalDay(2008, 9, 20, 12, 25, 40.104192000945),
              264.51782528,
              1e-10);
}

}  // namespace time_format_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
