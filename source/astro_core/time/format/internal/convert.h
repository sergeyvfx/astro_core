// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Low-level time and date conversion utilities, reusable by time formats.

#pragma once

#include <cassert>

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/internal/date_time_core.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_format_internal {

////////////////////////////////////////////////////////////////////////////////
// Conversion date-time to and from the Julian date format using a
// high-precision DoubleDouble value.
//
// References:
//
//   [Meeus1998] Meeus, Jean. Astronomical Algorithms. 2nd ed.,
//     Willmann-Bell, Inc., 1998.
//
//   [Vallado2013] Vallado, David A., and James Wertz.
//     Fundamentals of Astrodynamics and Applications. 2013.

// Convert Gregorian calendar date and time notation to the Julian date.
//
//   1 <= month <= 12
//   1 <= day <= 31
//   0 <= hour < 24
//   0 <= minute < 60
//   0 <= second < 60
//
// NOTE: Passing dates prior to the introduction of the Gregorian calendar in
// October 1582 will consider the date is in gregorian calendar.
inline auto DateTimeToJulianDate(const int year,
                                 const int month,
                                 const int day,
                                 const int hour,
                                 const int minute,
                                 const double seconds) -> DoubleDouble {
  // Algorithm from [Meeus1998], page 61.
  // It is also describe in the [Vallado2013] page 183.

  // Modify the month and year so that January and February are treated as
  // months 13 and 14, respectively.
  //
  // The naming follows [Vallado2013].
  int yr = year;
  int mo = month;
  if (mo == 1 || mo == 2) {
    yr = yr - 1;
    mo = mo + 12;
  }

  const double A = Floor(yr / 100.0);
  const double B = 2 - A + Floor(A / 4.0);
  const double C = ((seconds / 60.0 + minute) / 60.0 + hour) / 24.0;

  const double jd = Floor(365.25 * (yr + 4716)) + Floor(30.6001 * (mo + 1)) +
                    day + B - 1524.5;

  return {jd, C};
}

// Convert non-negative Julian date to date-time representation.
inline void JulianDateToDateTime(const DoubleDouble& jd,
                                 int& year,
                                 int& month,
                                 int& day,
                                 int& hour,
                                 int& minute,
                                 double& seconds) {
  // [Meeus1998], page 63.

  assert(jd >= 0);

  const DoubleDouble jd_off = jd + 0.5;

  const double Z = double(Trunc(jd_off));
  const double F = double(jd_off - Z);

  const double alpha = int((Z - 1867216.25) / 36524.25);
  const double A = Z + 1 + alpha - int(alpha / 4);

  const double B = A + 1524;
  const double C = int((B - 122.1) / 365.25);
  const double D = int(365.25 * C);
  const double E = int((B - D) / 30.6001);

  // Day of the month, which fractional indicates the time of the day.
  const double day_float = (B - D - int(30.6001 * E) + F);

  // Calculate year, month, day according to the algorithm from [Meeus1998].
  month = ((E < 14) ? E - 1 : E - 13);
  year = ((month > 2) ? C - 4716 : C - 4715);
  day = day_float;

  // Break down the fractional part of the day to the time in hours, minutes,
  // seconds.
  const double day_seconds = (day_float - day) * constants::kNumSecondsInDay;

  hour = int(day_seconds) / constants::kNumSecondsInHour;
  minute = (int(day_seconds) % int(constants::kNumSecondsInHour)) / 60;

  seconds = int(day_seconds) % 60 + (day_seconds - int(day_seconds));
}

////////////////////////////////////////////////////////////////////////////////
// Conversion from year-decimal-day to date-time and back.

// Convert the input_year and decimal_day within the year to date-time.
//
// If the decimal_day is above the number of days in the input_year then the
// year is incremented. Hence the output year can be different from the input
// year.
inline void YearDecimalDayToDateTime(const int input_year,
                                     const double decimal_day,
                                     int& year,
                                     int& month,
                                     int& day,
                                     int& hour,
                                     int& minute,
                                     double& seconds) {
  // Calculate the month and the actual year. Start with the first month of the
  // current year, and increment month until the number of decimal days is above
  // the number of days in the month.
  year = input_year;
  month = 1;
  double remaining_decimal_day = decimal_day;
  for (;;) {
    const int num_days_in_month = GetNumDaysInMonth(year, month);
    if (num_days_in_month > remaining_decimal_day) {
      break;
    }

    remaining_decimal_day -= num_days_in_month;
    ++month;

    // Wrap to the next year if needed.
    if (month == 13) {
      ++year;
      month = 1;
    }
  }

  day = remaining_decimal_day;

  const double day_seconds =
      (remaining_decimal_day - day) * constants::kNumSecondsInDay;

  hour = int(day_seconds) / constants::kNumSecondsInHour;
  minute = (int(day_seconds) % int(constants::kNumSecondsInHour)) / 60;

  seconds = int(day_seconds) % 60 + (day_seconds - int(day_seconds));
}

// Convert date-time to decimal day within the year.
constexpr auto DateTimeToDecimalDay(const int year,
                                    const int month,
                                    const int day,
                                    const int hour,
                                    const int minute,
                                    const double seconds) -> double {
  // Calculate the integral part of the decimal day by advancing all months
  // forward and taking the day-of-month into account.
  double decimal_day = day;
  for (int i = 1; i < month; ++i) {
    decimal_day += GetNumDaysInMonth(year, i);
  }

  decimal_day += double(hour) / 24.0 +
                 double(minute) / constants::kNumMinutesInDay +
                 double(seconds) / constants::kNumSecondsInDay;

  return decimal_day;
}

}  // namespace time_format_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
