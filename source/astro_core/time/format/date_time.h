// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// The DateTime representation is basically a tuple of (year, month, day, hour,
// minute, second, microsecond). This is a representation of a typical human
// readable date and time notation.
//
// The date is in Gregorian calendar.
//
// The following limits are expected:
//
//   1 <= month <= 12
//   1 <= day <= the number of days in the given month and year
//   0 <= hour < 24
//   0 <= minute < 60
//   0 <= second < 60
//   0 <= microsecond < 1000000
//
// All of the components of the date and time are integer values. Whenever the
// DateTime is constructed from a floating-point representation a
// round-half-to-even tiebreaker is used. This matches
//
// This makes it so conversion of time to the DateTime could be lossy, but
// matches the behavior to Python's datetime.datetime objects which makes it
// easier to cross-reference and port code over.

#pragma once

#include <cassert>

#include "astro_core/base/double_double.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/internal/convert.h"
#include "astro_core/time/format/internal/date_time_math.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class DateTime {
 public:
  DateTime() = default;

  constexpr DateTime(const int year,
                     const int month,
                     const int day,
                     const int hour = 0,
                     const int minute = 0,
                     const int second = 0,
                     const int microsecond = 0)
      : year_(year),
        month_(month),
        day_(day),
        hour_(hour),
        minute_(minute),
        second_(second),
        microsecond_(microsecond) {
    // TODO(sergey): Validate the values.
  }

  DateTime(const DateTime& other) = default;
  DateTime(DateTime&& other) noexcept = default;

  ~DateTime() = default;

  auto operator=(const DateTime& other) -> DateTime& = default;
  auto operator=(DateTime&& other) -> DateTime& = default;

  // Get date.
  constexpr auto GetYear() const -> int { return year_; }
  constexpr auto GetMonth() const -> int { return month_; }
  constexpr auto GetDay() const -> int { return day_; }

  // Get month as its short name: Jan, Feb, etc.
  constexpr auto GetMonthShortName() const -> const char* {
    assert(month_ >= 1);
    assert(month_ <= 12);

    if (month_ < 1 || month_ > 12) {
      return "";
    }

    // clang-format off
    const char* kMonths[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Now", "Dec"
    };
    // clang-format on

    return kMonths[month_ - 1];
  }

  // Get time.
  constexpr auto GetHour() const -> int { return hour_; }
  constexpr auto GetMinute() const -> int { return minute_; }
  constexpr auto GetSecond() const -> int { return second_; }
  constexpr auto GetMicrosecond() const -> int { return microsecond_; }

  // Comparison.
  constexpr auto operator<=>(const DateTime& other) const = default;
  constexpr auto operator==(const DateTime& other) const -> bool = default;

 private:
  int year_{0};
  int month_{1};
  int day_{1};

  int hour_{0};
  int minute_{0};
  int second_{0};
  int microsecond_{0};
};

// Print the DateTime to the stream.
// Prints the date time as a tuple, similar to Python's datetime.
inline auto operator<<(std::ostream& os, const DateTime& datetime)
    -> std::ostream& {
  os << "(" << datetime.GetYear() << ", " << datetime.GetMonth() << ", "
     << datetime.GetDay() << ", " << datetime.GetHour() << ", "
     << datetime.GetMinute() << ", " << datetime.GetSecond() << ", "
     << datetime.GetMicrosecond() << ")";
  return os;
}

template <>
struct TimeFormatTraits<DateTime> {
  static auto ToJulianDate(const DateTime& datetime) -> DoubleDouble {
    return time_format_internal::DateTimeToJulianDate(
        datetime.GetYear(),
        datetime.GetMonth(),
        datetime.GetDay(),
        datetime.GetHour(),
        datetime.GetMinute(),
        datetime.GetSecond() + double(datetime.GetMicrosecond()) / 1000000);
  }

  static auto FromJulianDate(const DoubleDouble& jd) -> DateTime {
    int year, month, day, hour, minute;
    double seconds;

    time_format_internal::JulianDateToDateTime(
        jd, year, month, day, hour, minute, seconds);

    int second = seconds;

    // Use round-half-to-even tiebreaker to match Python datetime.
    int microsecond = RoundHalfToEven((seconds - second) * 1000000);

    // If the microseconds rounded to a full second, set microseconds to 0 and
    // advance the second. This will also take care of advancing seconds,
    // minutes and so on when needed.
    if (microsecond >= 1000000) {
      microsecond -= 1000000;
      time_format_internal::DateTimeAdvanceSecond(
          year, month, day, hour, minute, second);
    }

    return DateTime(year, month, day, hour, minute, second, microsecond);
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
