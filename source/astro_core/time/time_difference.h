// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// DIfference between two points in time in the same scale.
// Provides a convenient way of advancing time.

#pragma once

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class TimeDifference {
 public:
  TimeDifference() = default;

  // Construct time difference object from the given difference in the number of
  // days.
  static constexpr auto FromDays(const DoubleDouble& num_days)
      -> TimeDifference {
    TimeDifference time_difference;
    time_difference.difference_jd_ = num_days;
    return time_difference;
  }

  // Construct time difference object from the given difference in seconds.
  static constexpr auto FromSeconds(const DoubleDouble& difference_seconds)
      -> TimeDifference {
    TimeDifference time_difference;
    time_difference.difference_jd_ =
        difference_seconds / constants::kNumSecondsInDay;
    return time_difference;
  }

  // Represent the time difference in the number of days.
  constexpr auto InDays() const -> DoubleDouble { return difference_jd_; }

  // Represent the time difference in the number of seconds.
  constexpr auto InSeconds() const -> DoubleDouble {
    return InDays() * constants::kNumSecondsInDay;
  }

 private:
  // Difference between two Julian date, measured in julian days.
  DoubleDouble difference_jd_{0};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
