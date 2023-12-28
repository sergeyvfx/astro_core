// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/internal/date_time_math.h"

#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_format_internal {

TEST(DateTimeMath, DateTimeAdvanceDay) {
  {
    int year = 1900;
    int month = 2;
    int day = 28;
    DateTimeAdvanceDay(year, month, day);

    EXPECT_EQ(year, 1900);
    EXPECT_EQ(month, 3);
    EXPECT_EQ(day, 1);
  }

  {
    int year = 2000;
    int month = 2;
    int day = 28;
    DateTimeAdvanceDay(year, month, day);

    EXPECT_EQ(year, 2000);
    EXPECT_EQ(month, 2);
    EXPECT_EQ(day, 29);
  }

  {
    int year = 2000;
    int month = 12;
    int day = 31;
    DateTimeAdvanceDay(year, month, day);

    EXPECT_EQ(year, 2001);
    EXPECT_EQ(month, 1);
    EXPECT_EQ(day, 1);
  }
}

TEST(DateTimeMath, DateTimeAdvanceSecond) {
  // Simple case: only increment the second.
  {
    int year = 2000;
    int month = 12;
    int day = 31;
    int hour = 12;
    int minute = 30;
    int second = 45;

    DateTimeAdvanceSecond(year, month, day, hour, minute, second);

    EXPECT_EQ(year, 2000);
    EXPECT_EQ(month, 12);
    EXPECT_EQ(day, 31);
    EXPECT_EQ(hour, 12);
    EXPECT_EQ(minute, 30);
    EXPECT_EQ(second, 46);
  }

  // Advance into the minute.
  {
    int year = 2000;
    int month = 12;
    int day = 31;
    int hour = 12;
    int minute = 30;
    int second = 59;

    DateTimeAdvanceSecond(year, month, day, hour, minute, second);

    EXPECT_EQ(year, 2000);
    EXPECT_EQ(month, 12);
    EXPECT_EQ(day, 31);
    EXPECT_EQ(hour, 12);
    EXPECT_EQ(minute, 31);
    EXPECT_EQ(second, 0);
  }

  // Advance into the hour.
  {
    int year = 2000;
    int month = 12;
    int day = 31;
    int hour = 12;
    int minute = 59;
    int second = 59;

    DateTimeAdvanceSecond(year, month, day, hour, minute, second);

    EXPECT_EQ(year, 2000);
    EXPECT_EQ(month, 12);
    EXPECT_EQ(day, 31);
    EXPECT_EQ(hour, 13);
    EXPECT_EQ(minute, 0);
    EXPECT_EQ(second, 0);
  }

  // Advance into the date.
  {
    int year = 2000;
    int month = 12;
    int day = 31;
    int hour = 23;
    int minute = 59;
    int second = 59;

    DateTimeAdvanceSecond(year, month, day, hour, minute, second);

    EXPECT_EQ(year, 2001);
    EXPECT_EQ(month, 1);
    EXPECT_EQ(day, 1);
    EXPECT_EQ(hour, 0);
    EXPECT_EQ(minute, 0);
    EXPECT_EQ(second, 0);
  }
}

}  // namespace time_format_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
