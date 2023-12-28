// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Time format which interfaces std::chrono::system_clock.
//
// Allows to use the system clock from the STL as a source of time point.

#pragma once

#include <chrono>
#include <ostream>

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class SystemClock {
 public:
  using TimePoint = std::chrono::system_clock::time_point;

  SystemClock() = default;

  // Construct system clock object from the given time point provided by the
  // STL.
  constexpr explicit SystemClock(const TimePoint& time_point)
      : time_point_(time_point) {}

  SystemClock(const SystemClock& other) = default;
  SystemClock(SystemClock&& other) noexcept = default;

  ~SystemClock() = default;

  auto operator=(const SystemClock& other) -> SystemClock& = default;
  auto operator=(SystemClock&& other) -> SystemClock& = default;

  // Get the STL's system_clock::time_point representation.
  explicit constexpr operator TimePoint() const { return time_point_; }

  // Convert this date to time_t representation.
  explicit operator std::time_t() const {
    return std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::time_point(time_point_));
  }

  // Comparison.
  constexpr auto operator<=>(const SystemClock& other) const -> int {
    // TODO(sergey): Replace with <=> operator for the time point when it is
    // supported in Xcode.

    using Duration = TimePoint::duration;

    const TimePoint::duration difference =
        time_point_.time_since_epoch() - other.time_point_.time_since_epoch();

    if (difference < Duration(0)) {
      return -1;
    }

    if (difference > Duration(0)) {
      return 1;
    }

    return 0;
  }
  constexpr auto operator==(const SystemClock& other) const -> bool = default;

 private:
  TimePoint time_point_;
};

// Print the system clock.
// Prints the value as a number of seconds since the Unix epoch.
inline auto operator<<(std::ostream& os, const SystemClock& system_clock)
    -> std::ostream& {
  os << std::time_t(system_clock);
  return os;
}

namespace system_clock_internal {

// Utility function to convert the Unix time denoted in seconds since the
// Unix epoch to std::chrono::system_clock::time_point.
//
// Relies on the std::chrono::system_clock using the Unix epoch as its epoch.
// This is required by the C++20: system_clock measures Unix Time (i.e., time
// since 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970,
// not counting leap seconds).
inline auto TimePointFromUnixTime(const double unix_time)
    -> std::chrono::system_clock::time_point {
  namespace chrono = std::chrono;

  return chrono::system_clock::time_point(
      chrono::duration_cast<chrono::system_clock::duration>(
          chrono::duration<double>(unix_time)));
}

}  // namespace system_clock_internal

// Conversion to and from the internal time representation which stores time as
// a high-precision DoubleDouble Julian date value.
template <>
struct TimeFormatTraits<SystemClock> {
  static constexpr auto ToJulianDate(const SystemClock& system_clock)
      -> DoubleDouble {
    using TimePoint = SystemClock::TimePoint;
    using Seconds = std::chrono::duration<double>;
    using std::chrono::time_point_cast;

    return DoubleDouble(time_point_cast<Seconds>(TimePoint(system_clock))
                            .time_since_epoch()
                            .count()) /
               constants::kNumSecondsInDay +
           constants::kJulianDateEpochUnixEpoch;
  }

  static auto FromJulianDate(const DoubleDouble& jd) -> SystemClock {
    return SystemClock(system_clock_internal::TimePointFromUnixTime(
        double((jd - constants::kJulianDateEpochUnixEpoch) *
               constants::kNumSecondsInDay)));
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
