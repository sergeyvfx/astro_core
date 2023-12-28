// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Representation of time.
//
// It can be constructed from various time formats (such as Unix time, or
// Modified Julian Time).
//
// The time is aware of the time scale, which denotes how time flows. Depending
// on the scale, this makes Time objects aware of the leap seconds.

#pragma once

#include <cassert>

#include "astro_core/base/double_double.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/time/scale.h"
#include "astro_core/time/time_difference.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Time {
 public:
  Time() = default;

  // Construct the Time object from the given time point in a custom format, and
  // the time scale.
  template <class FormatType>
  constexpr Time(const FormatType& time, const TimeScale scale)
      : scale_(scale), jd_(TimeFormatTraits<FormatType>::ToJulianDate(time)) {}

  Time(const Time& other) = default;
  Time(Time&& other) noexcept = default;

  ~Time() = default;

  auto operator=(const Time& other) -> Time& = default;
  auto operator=(Time&& other) -> Time& = default;

  // Get scale used to measure this time.
  constexpr auto GetScale() const -> TimeScale { return scale_; }

  // Get time value in the given format.
  //
  // This is just a presentation of the internal format in a more suitable
  // format. No scale conversion happens here.
  template <class FormatType>
  constexpr auto AsFormat() const -> FormatType {
    return TimeFormatTraits<FormatType>::FromJulianDate(jd_);
  }

  // Construct a new time object which points to the same time point but in
  // the different time scale.
  auto ToScale(TimeScale scale) const -> Time;

  // Construct a new time object which points to the same time point but in
  // the different time scale.
  // The API mimics the AsFormat<> and provisions some possible compile-time
  // optimizations.
  template <TimeScale kToScale>
  auto ToScale() const -> Time {
    return ToScale(kToScale);
  }

  // Check for an exact equality of scale and time point.
  auto operator==(const Time& other) const -> bool {
    return scale_ == other.scale_ && jd_ == other.jd_;
  }
  auto operator!=(const Time& other) const -> bool { return !(*this == other); }

  auto operator+=(const TimeDifference& time_difference) -> Time& {
    jd_ += time_difference.InDays();
    return *this;
  }
  auto operator-=(const TimeDifference& time_difference) -> Time& {
    jd_ -= time_difference.InDays();
    return *this;
  }

 private:
  TimeScale scale_{TimeScale::kUTC};

  // Internal time representation stored as a Julian date in a high-precision
  // scalar.
  DoubleDouble jd_{0};
};

inline auto operator+(const Time& time, const TimeDifference& time_difference)
    -> Time {
  Time result = time;
  result += time_difference;
  return result;
}

inline auto operator-(const Time& time, const TimeDifference& time_difference)
    -> Time {
  Time result = time;
  result -= time_difference;
  return result;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
