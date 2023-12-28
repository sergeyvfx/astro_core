// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/date_time.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

TEST(DateTime, Construct) {
  {
    const DateTime datetime(1970, 5, 18, 16, 32, 48, 512);

    EXPECT_EQ(datetime.GetYear(), 1970);
    EXPECT_EQ(datetime.GetMonth(), 5);
    EXPECT_EQ(datetime.GetDay(), 18);

    EXPECT_EQ(datetime.GetHour(), 16);
    EXPECT_EQ(datetime.GetMinute(), 32);
    EXPECT_EQ(datetime.GetSecond(), 48);
    EXPECT_EQ(datetime.GetMicrosecond(), 512);
  }
}

TEST(DateTime, Compare) {
  EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) ==
              DateTime(1970, 5, 18, 16, 32, 48, 512));
  EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) ==
               DateTime(1970, 5, 18, 16, 32, 48, 128));

  // operator<
  {
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1970, 5, 18, 16, 32, 48, 768));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1970, 5, 18, 16, 32, 48, 256));

    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1970, 5, 18, 16, 32, 49, 512));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1970, 5, 18, 16, 32, 47, 512));

    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1970, 5, 18, 16, 33, 48, 512));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1970, 5, 18, 16, 31, 48, 512));

    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1970, 5, 18, 17, 32, 48, 512));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1970, 5, 18, 15, 32, 48, 512));

    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1970, 5, 19, 16, 32, 48, 512));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1970, 5, 17, 16, 32, 48, 512));

    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1970, 6, 18, 16, 32, 48, 512));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1970, 4, 18, 16, 32, 48, 512));

    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                DateTime(1971, 5, 18, 16, 32, 48, 512));
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) <
                 DateTime(1969, 5, 18, 16, 32, 48, 512));
  }

  // operator>
  {
    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1970, 5, 18, 16, 32, 48, 768));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1970, 5, 18, 16, 32, 48, 256));

    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1970, 5, 18, 16, 32, 49, 512));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1970, 5, 18, 16, 32, 47, 512));

    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1970, 5, 18, 16, 33, 48, 512));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1970, 5, 18, 16, 31, 48, 512));

    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1970, 5, 18, 17, 32, 48, 512));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1970, 5, 18, 15, 32, 48, 512));

    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1970, 5, 19, 16, 32, 48, 512));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1970, 5, 17, 16, 32, 48, 512));

    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1970, 6, 18, 16, 32, 48, 512));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1970, 4, 18, 16, 32, 48, 512));

    EXPECT_FALSE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                 DateTime(1971, 5, 18, 16, 32, 48, 512));
    EXPECT_TRUE(DateTime(1970, 5, 18, 16, 32, 48, 512) >
                DateTime(1969, 5, 18, 16, 32, 48, 512));
  }
}

TEST(DateTime, GetMonthShortName) {
  EXPECT_STREQ(DateTime(2022, 12, 1).GetMonthShortName(), "Dec");
}

TEST(DateTime, Print) {
  std::stringstream stream;
  stream << DateTime(1970, 5, 18, 16, 32, 48, 512);
  EXPECT_EQ(stream.str(), "(1970, 5, 18, 16, 32, 48, 512)");
}

TEST(DateTime, ToJulianDate) {
  using FormatTraits = TimeFormatTraits<DateTime>;

  // Assume that the implementation uses DateTimeToJulianDate() which has its
  // own intensive tests, so only perform test to catch obvious issues.

  // [Vallado2013] example 3-4.
  //
  // Verified and expanded the precision for comparision using Astropy:
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(1996, 10, 26, 14, 20, 0))
  //   >>> (t.jd1, t.jd2)
  //   (2450383.0, 0.09722222222222221)
  EXPECT_THAT(FormatTraits::ToJulianDate(DateTime(1996, 10, 26, 14, 20, 0)),
              NearUsingAbsDifferenceMetric(
                  DoubleDouble(2450383, 0.0972222222222222), 1e-12));
}

TEST(DateTime, FromJulianDate) {
  using FormatTraits = TimeFormatTraits<DateTime>;

  // Assume that the implementation uses JulianDateToDateTime() which has its
  // own intensive tests, so only perform test to catch obvious issues.

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
    DateTime datetime = FormatTraits::FromJulianDate(2436116.31);

    EXPECT_EQ(datetime.GetYear(), 1957);
    EXPECT_EQ(datetime.GetMonth(), 10);
    EXPECT_EQ(datetime.GetDay(), 4);
    EXPECT_EQ(datetime.GetHour(), 19);
    EXPECT_EQ(datetime.GetMinute(), 26);
    EXPECT_EQ(datetime.GetSecond(), 24);
    EXPECT_EQ(datetime.GetMicrosecond(), 5);
  }
}

}  // namespace astro_core
