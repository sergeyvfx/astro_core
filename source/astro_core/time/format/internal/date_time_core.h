// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Mathematic operating on date time format representation.
//
// TODO(sergey): Find a more reusable API here. Currently it is optimized for
// the need of rounding date-time milliseconds up.

#pragma once

#include <cassert>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_format_internal {

// Get the number of days in the given month of a given year.
// Month is measured from 1 to 12 (January to December respectively).
constexpr auto GetNumDaysInMonth(const int year, const int month) -> int {
  assert(month >= 1);
  assert(month <= 12);

  if (month == 2) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
      return 29;
    }
    return 28;
  }

  constexpr int kNumDaysInMonth[12] = {
      31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  return kNumDaysInMonth[month - 1];
}

}  // namespace time_format_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
