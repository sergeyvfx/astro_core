// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Astronomical Julian date
//
// Source: https://en.wikipedia.org/wiki/Julian_day
//
//   The Julian day is the continuous count of days since the beginning of the
//   Julian period.
//
//   The Julian day number (JDN) is the integer assigned to a whole solar day
//   in the Julian day count starting from noon Universal Time, with Julian day
//   number 0 assigned to the day starting at noon on Monday, January 1, 4713
//   BC, proleptic Julian calendar (November 24, 4714 BC, in the proleptic
//   Gregorian calendar),
//
//   The Julian date (JD) of any instant is the Julian day number plus the
//   fraction of a day since the preceding noon in Universal Time.
//
// Epoch: 12:00 January 1, 4713 BC proleptic Julian calendar.
//
// The JulianDate implements the JD format where the JDN and the fractional part
// of the JDN are stored in the separate fields. Such separation allows to get
// the maximum of the floating point precision since the modern JDN is greater
// than 2450000 which only leaves 1e-9 precision for the fractional part.
//
// For example, 2451544.5 in JD is midnight on January 1, 2000.
// The the Unix Epoch of January 1 1970 is 2451544.5 JD.
//
// The JulianDate class implements basic arithmetic operations (such as
// addition, subtraction, multiplication, division) for JulianDate and double
// and DoubleDouble type of operands.

#pragma once

#include "astro_core/base/double_double.h"
#include "astro_core/time/format/arithmetic.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class JulianDate : public ArithmeticFormat<DoubleDouble, JulianDate> {
  using BaseClass = ArithmeticFormat<DoubleDouble, JulianDate>;

 public:
  JulianDate() = default;

  // Construct julian date object from a single high-precision scalar
  // representation.
  constexpr explicit JulianDate(const DoubleDouble& jd) : BaseClass(jd) {}

  // Construct julian date object from an unevaluated sum of two scalars.
  constexpr explicit JulianDate(const double jd1, const double jd2)
      : BaseClass(DoubleDouble(jd1, jd2)) {}

  using BaseClass::operator<=>;
  using BaseClass::operator==;

  // Get hi and low values of the Julian date.
  // The actual Julian date is jd1 + jd2.
  constexpr inline auto GetJD1() const -> double { return value_.GetHi(); }
  constexpr inline auto GetJD2() const -> double { return value_.GetLo(); }
};

// Print the Julian date to the stream.
// Prints as a value in JD followed with the format designator "JD".
inline auto operator<<(std::ostream& os, const JulianDate& jd)
    -> std::ostream& {
  os << DoubleDouble(jd) << " JD";
  return os;
}

// Conversion to and from the internal time representation which stores time as
// a high-precision DoubleDouble Julian date value.
template <>
struct TimeFormatTraits<JulianDate> {
  static constexpr auto ToJulianDate(const JulianDate& jd) -> DoubleDouble {
    return DoubleDouble(jd);
  }

  static constexpr auto FromJulianDate(const DoubleDouble& jd) -> JulianDate {
    return JulianDate(jd);
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
