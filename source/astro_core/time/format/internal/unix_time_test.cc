// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/unix_time.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

TEST(UnixTime, Construct) {
  {
    const UnixTime unix_time(1234.56);
    EXPECT_THAT(DoubleDouble(unix_time),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }

  {
    const UnixTime unix_time(1234.0, 0.56);
    EXPECT_THAT(DoubleDouble(unix_time),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }
}

TEST(UnixTime, Compare) {
  EXPECT_TRUE(UnixTime(100) == UnixTime(100));
  EXPECT_FALSE(UnixTime(100) == UnixTime(200));

  EXPECT_FALSE(UnixTime(100) != UnixTime(100));
  EXPECT_TRUE(UnixTime(100) != UnixTime(200));

  EXPECT_TRUE(UnixTime(100) < UnixTime(200));
  EXPECT_FALSE(UnixTime(200) < UnixTime(100));

  EXPECT_FALSE(UnixTime(100) > UnixTime(200));
  EXPECT_TRUE(UnixTime(200) > UnixTime(100));
}

TEST(UnixTime, ToJulianDate) {
  using FormatTraits = TimeFormatTraits<UnixTime>;

  // Unix epoch in Julian date.
  //
  //   >>> from astropy.time import Time
  //   >>> Time(0, format='unix').jd
  //   2440587.5
  EXPECT_THAT(FormatTraits::ToJulianDate(UnixTime(0)),
              NearUsingAbsDifferenceMetric(DoubleDouble(2440587.5), 1e-12));

  // Value from the example of the Wikipedia page about the Unix time.
  // This is what Wikipedia showed as a current Unix time at the moment of this
  // code being written.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(1667662059, format='unix')
  //   >>> (t.jd1, t.jd2)
  //   (2459889.0, 0.1442013888888889)
  EXPECT_THAT(FormatTraits::ToJulianDate(UnixTime(1667662059)),
              NearUsingAbsDifferenceMetric(
                  DoubleDouble(2459889.0, 0.1442013888888889), 1e-12));
}

TEST(UnixTime, FromJulianDate) {
  using FormatTraits = TimeFormatTraits<UnixTime>;

  // Reverse tests of the ToJulianDate().

  EXPECT_THAT(DoubleDouble(FormatTraits::FromJulianDate(2440587.5)),
              NearUsingAbsDifferenceMetric(DoubleDouble(0), 1e-12));

  EXPECT_THAT(DoubleDouble(FormatTraits::FromJulianDate(
                  DoubleDouble(2459889.0, 0.1442013888888889))),
              NearUsingAbsDifferenceMetric(DoubleDouble(1667662059), 1e-12));
}

TEST(UnixTime, Print) {
  std::stringstream stream;
  stream << UnixTime(1234.0, 0.56);
  EXPECT_EQ(stream.str(), "1234.56");
}

}  // namespace astro_core
