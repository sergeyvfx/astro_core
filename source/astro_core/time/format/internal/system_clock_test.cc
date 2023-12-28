// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/system_clock.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

using system_clock_internal::TimePointFromUnixTime;

TEST(SystemClock, Construct) {
  using TimePoint = SystemClock::TimePoint;
  {
    const TimePoint time_point = TimePointFromUnixTime(1234);

    const SystemClock system_clock(time_point);
    EXPECT_EQ(TimePoint(system_clock), time_point);
  }
}

TEST(SystemClock, Compare) {
  EXPECT_TRUE(SystemClock(TimePointFromUnixTime(100)) ==
              SystemClock(TimePointFromUnixTime(100)));
  EXPECT_FALSE(SystemClock(TimePointFromUnixTime(100)) ==
               SystemClock(TimePointFromUnixTime(200)));

  EXPECT_FALSE(SystemClock(TimePointFromUnixTime(100)) !=
               SystemClock(TimePointFromUnixTime(100)));
  EXPECT_TRUE(SystemClock(TimePointFromUnixTime(100)) !=
              SystemClock(TimePointFromUnixTime(200)));

  EXPECT_TRUE(SystemClock(TimePointFromUnixTime(100)) <
              SystemClock(TimePointFromUnixTime(200)));
  EXPECT_FALSE(SystemClock(TimePointFromUnixTime(200)) <
               SystemClock(TimePointFromUnixTime(100)));

  EXPECT_FALSE(SystemClock(TimePointFromUnixTime(100)) >
               SystemClock(TimePointFromUnixTime(200)));
  EXPECT_TRUE(SystemClock(TimePointFromUnixTime(200)) >
              SystemClock(TimePointFromUnixTime(100)));
}

TEST(SystemClock, Print) {
  const SystemClock::TimePoint time_point = TimePointFromUnixTime(1234);

  std::stringstream stream;
  stream << SystemClock(time_point);
  EXPECT_EQ(stream.str(), "1234");
}

TEST(SystemClock, ToJulianDate) {
  using FormatTraits = TimeFormatTraits<SystemClock>;

  // Unix epoch in Julian date.
  //
  //   >>> from astropy.time import Time
  //   >>> Time(0, format='unix').jd
  //   2440587.5
  EXPECT_THAT(FormatTraits::ToJulianDate(SystemClock(TimePointFromUnixTime(0))),
              NearUsingAbsDifferenceMetric(DoubleDouble(2440587.5), 1e-12));

  // Value from the example of the Wikipedia page about the Unix time.
  // This is what Wikipedia showed as a current Unix time at the moment of this
  // code being written.
  //
  //   >>> from astropy.time import Time
  //   >>> t = Time(1667662059, format='unix')
  //   >>> (t.jd1, t.jd2)
  //   (2459889.0, 0.1442013888888889)
  EXPECT_THAT(FormatTraits::ToJulianDate(
                  SystemClock(TimePointFromUnixTime(1667662059))),
              NearUsingAbsDifferenceMetric(
                  DoubleDouble(2459889.0, 0.1442013888888889), 1e-12));
}

TEST(SystemClock, FromJulianDate) {
  using Seconds = std::chrono::duration<double>;
  using std::chrono::time_point_cast;

  using TimePoint = SystemClock::TimePoint;
  using FormatTraits = TimeFormatTraits<SystemClock>;

  // Reverse tests of the ToJulianDate().

  EXPECT_NEAR(time_point_cast<Seconds>(
                  TimePoint(FormatTraits::FromJulianDate(2440587.5)))
                  .time_since_epoch()
                  .count(),
              0,
              1e-12);

  EXPECT_NEAR(time_point_cast<Seconds>(
                  TimePoint(FormatTraits::FromJulianDate(
                      DoubleDouble(2459889.0, 0.1442013888888889))))
                  .time_since_epoch()
                  .count(),
              1667662059.0,
              1e-12);
}

}  // namespace astro_core
