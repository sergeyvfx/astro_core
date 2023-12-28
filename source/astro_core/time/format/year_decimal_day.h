// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Date-time representation which uses integral value to store a Gregorian year
// number and a fractional value for the day number since the beginning of the
// year. The integer values of the day correspond to midnight.
//
// Such format is used, for example, by TLE.

#pragma once

#include <ostream>

#include "astro_core/base/double_double.h"
#include "astro_core/time/format/internal/convert.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class YearDecimalDay {
 public:
  YearDecimalDay() = default;

  // Construct the time from the given integral year and a decimal day within
  // that year.
  // The decimal day can exceed the number of days in the year. The dowside of
  // relying on this is possible loss of precision.
  constexpr YearDecimalDay(const int year, const double decimal_day) {
    SetYear(year);
    SetDecimalDay(decimal_day);
  }

  YearDecimalDay(const YearDecimalDay& other) = default;
  YearDecimalDay(YearDecimalDay&& other) noexcept = default;

  ~YearDecimalDay() = default;

  auto operator=(const YearDecimalDay& other) -> YearDecimalDay& = default;
  auto operator=(YearDecimalDay&& other) -> YearDecimalDay& = default;

  constexpr auto GetYear() const -> int { return year_; }
  constexpr void SetYear(const int year) { year_ = year; }

  // The decimal day can exceed the number of days in the year. The downside of
  // relying on this is possible loss of precision.
  constexpr auto GetDecimalDay() const -> double { return decimal_day_; }
  constexpr void SetDecimalDay(const double decimal_day) {
    decimal_day_ = decimal_day;
  }

  // Comparison.
  constexpr auto operator<=>(const YearDecimalDay& other) const = default;
  constexpr auto operator==(const YearDecimalDay& other) const
      -> bool = default;

 private:
  int year_{0};
  double decimal_day_{0};
};

// Print the Unix time as a number of second since Unix epoch
inline auto operator<<(std::ostream& os, const YearDecimalDay& year_decimal_day)
    -> std::ostream& {
  os << year_decimal_day.GetYear() << ":" << year_decimal_day.GetDecimalDay();
  return os;
}

// Conversion to and from the internal time representation which stores time as
// a high-precision DoubleDouble Julian date value.
template <>
struct TimeFormatTraits<YearDecimalDay> {
  static auto ToJulianDate(const YearDecimalDay& year_decimal_day) {
    int year, month, day, hour, minute;
    double seconds;
    time_format_internal::YearDecimalDayToDateTime(
        year_decimal_day.GetYear(),
        year_decimal_day.GetDecimalDay(),
        year,
        month,
        day,
        hour,
        minute,
        seconds);

    return time_format_internal::DateTimeToJulianDate(
        year, month, day, hour, minute, seconds);
  }

  static auto FromJulianDate(const DoubleDouble& jd) -> YearDecimalDay {
    int year, month, day, hour, minute;
    double seconds;

    time_format_internal::JulianDateToDateTime(
        jd, year, month, day, hour, minute, seconds);

    const double decimal_day = time_format_internal::DateTimeToDecimalDay(
        year, month, day, hour, minute, seconds);

    return YearDecimalDay(year, decimal_day);
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
