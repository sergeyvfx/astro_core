// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Mathematic operating on date time format representation.
//
// TODO(sergey): Find a more reusable API here. Currently it is optimized for
// the need of rounding date-time milliseconds up.

#pragma once

#include <cassert>

#include "astro_core/time/format/internal/date_time_core.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_format_internal {

// Advance the date one day into the future.
constexpr void DateTimeAdvanceDay(int& year, int& month, int& day) {
  day += 1;
  if (day <= GetNumDaysInMonth(year, month)) {
    return;
  }

  day = 1;
  month += 1;
  if (month <= 12) {
    return;
  }

  month = 1;
  year += 1;
}

// Advance the date-time one second into the future.
constexpr void DateTimeAdvanceSecond(
    int& year, int& month, int& day, int& hour, int& minute, int& second) {
  second += 1;
  if (second < 60) {
    return;
  }

  second -= 60;
  minute += 1;
  if (minute < 60) {
    return;
  }

  minute -= 60;
  hour += 1;
  if (hour < 24) {
    return;
  }

  hour -= 24;
  DateTimeAdvanceDay(year, month, day);
}

}  // namespace time_format_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
