// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Unix time is measured in the number of seconds since 1970-01-01 00:00:00 UTC
// (which is often known as an Unix epoch).
//
// Note that this is not a true representation of the UTC, because leap seconds
// are not independently represented. Instead, every day in Unix time contains
// exactly 86400 seconds, with leap seconds sharing the same timestamp as the
// second immediately before or after them.

#pragma once

#include <ostream>

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/time/format/arithmetic.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class UnixTime : public ArithmeticFormat<DoubleDouble, UnixTime> {
  using BaseClass = ArithmeticFormat<DoubleDouble, UnixTime>;

 public:
  UnixTime() = default;

  // Construct Unix time object from a single high-precision scalar
  // representation.
  constexpr explicit UnixTime(const DoubleDouble unix_timestamp)
      : BaseClass(unix_timestamp) {}

  // Construct Unix time object from an unevaluated sum of two scalars.
  constexpr explicit UnixTime(const double jd1, const double jd2)
      : BaseClass(DoubleDouble(jd1, jd2)) {}

  using BaseClass::operator<=>;
  using BaseClass::operator==;

  // Get hi and low values of the Unix time.
  // The actual Unix time is jd1 + jd2.
  constexpr inline auto GetJD1() const -> double { return value_.GetHi(); }
  constexpr inline auto GetJD2() const -> double { return value_.GetLo(); }
};

// Print the Unix time as a number of second since Unix epoch
inline auto operator<<(std::ostream& os, const UnixTime& unix_time)
    -> std::ostream& {
  os << DoubleDouble(unix_time);
  return os;
}

// Conversion to and from the internal time representation which stores time as
// a high-precision DoubleDouble Julian date value.
template <>
struct TimeFormatTraits<UnixTime> {
  static constexpr auto ToJulianDate(const UnixTime& unix_time)
      -> DoubleDouble {
    return DoubleDouble(unix_time) / constants::kNumSecondsInDay +
           constants::kJulianDateEpochUnixEpoch;
  }

  static constexpr auto FromJulianDate(const DoubleDouble& jd) -> UnixTime {
    return UnixTime((jd - constants::kJulianDateEpochUnixEpoch) *
                    constants::kNumSecondsInDay);
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
