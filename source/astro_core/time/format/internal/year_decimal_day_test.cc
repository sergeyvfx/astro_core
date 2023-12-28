// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/year_decimal_day.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

TEST(YearDecimalDay, Construct) {
  {
    const YearDecimalDay year_decimal_day(1234, 56.78);
    EXPECT_THAT(year_decimal_day.GetYear(), 1234);
    EXPECT_THAT(year_decimal_day.GetDecimalDay(), 56.78);
  }
}

TEST(YearDecimalDay, Compare) {
  EXPECT_TRUE(YearDecimalDay(1234, 56.78) == YearDecimalDay(1234, 56.78));
  EXPECT_FALSE(YearDecimalDay(1234, 56.78) == YearDecimalDay(4321, 56.78));
  EXPECT_FALSE(YearDecimalDay(1234, 56.78) == YearDecimalDay(1234, 78.56));

  EXPECT_FALSE(YearDecimalDay(1234, 56.78) != YearDecimalDay(1234, 56.78));
  EXPECT_TRUE(YearDecimalDay(1234, 56.78) != YearDecimalDay(4321, 56.78));
  EXPECT_TRUE(YearDecimalDay(1234, 56.78) != YearDecimalDay(1234, 78.56));

  EXPECT_TRUE(YearDecimalDay(1234, 56.78) < YearDecimalDay(1234, 57));
  EXPECT_TRUE(YearDecimalDay(1234, 56.78) < YearDecimalDay(1235, 56.78));
  EXPECT_FALSE(YearDecimalDay(1234, 57) < YearDecimalDay(1234, 56.78));
  EXPECT_FALSE(YearDecimalDay(1235, 56.78) < YearDecimalDay(1234, 56.78));

  EXPECT_FALSE(YearDecimalDay(1234, 56.78) > YearDecimalDay(1234, 57));
  EXPECT_FALSE(YearDecimalDay(1234, 56.78) > YearDecimalDay(1235, 56.78));
  EXPECT_TRUE(YearDecimalDay(1234, 57) > YearDecimalDay(1234, 56.78));
  EXPECT_TRUE(YearDecimalDay(1235, 56.78) > YearDecimalDay(1234, 56.78));
}

TEST(YearDecimalDay, Print) {
  std::stringstream stream;
  stream << YearDecimalDay(1234, 56.78);
  EXPECT_EQ(stream.str(), "1234:56.78");
}

TEST(YearDecimalDay, ToJulianDate) {
  using FormatTraits = TimeFormatTraits<YearDecimalDay>;

  // The date-time of YearDecimalDay(2008, 264.51782528)) is
  // 2008-09-20 12:25:40.104192000945 which is confirmed by the SGP4 library and
  // verified by the conversion utilities tests.
  //
  // The value of 2454730.0, 0.017825279999999943 is obtained via astropy for
  // the named date-time:
  //
  //   >>> from astropy.time import Time
  //   >>> from datetime import datetime
  //   >>> t = Time(datetime(2008, 9, 20, 12, 25, 40, 104192))
  //   >>> t.jd1, t.jd2
  //   (2454730.0, 0.017825279999999943)
  EXPECT_THAT(FormatTraits::ToJulianDate(YearDecimalDay(2008, 264.51782528)),
              NearUsingAbsDifferenceMetric(
                  DoubleDouble(2454730.0, 0.017825279999999943), 1e-12));
}

TEST(YearDecimalDay, FromJulianDate) {
  using FormatTraits = TimeFormatTraits<YearDecimalDay>;

  // Reverse of the ToJulianDate() test.

  {
    const YearDecimalDay year_decimal_day = FormatTraits::FromJulianDate(
        DoubleDouble(2454730.0, 0.017825279999999943));

    EXPECT_EQ(year_decimal_day.GetYear(), 2008);
    EXPECT_NEAR(year_decimal_day.GetDecimalDay(), 264.51782528, 1e-12);
  }
}

}  // namespace astro_core
