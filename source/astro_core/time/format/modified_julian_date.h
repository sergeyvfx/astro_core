// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Modified Julian date (MJD)
//
// This is a variant of the astronomical julian date with an epoch at
// 0:00 November 17, 1858. The MJD can be calculated as `JD - 2400000.5`.
//
// The goat of the offset is to lower the values of the current timepoints,
// enabling higher precision.
//
// For example, 51544.0 in MJD is midnight on January 1, 2000.
// The the Unix Epoch of January 1 1970 is 51544 MJD.
//
// The ModifiedJulianDate class implements basic arithmetic operations (such as
// addition, subtraction, multiplication, division) for ModifiedJulianDate and
// double and DoubleDouble type of operands.

#pragma once

#include <ostream>

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/time/format/arithmetic.h"
#include "astro_core/time/format/traits.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ModifiedJulianDate
    : public ArithmeticFormat<DoubleDouble, ModifiedJulianDate> {
  using BaseClass = ArithmeticFormat<DoubleDouble, ModifiedJulianDate>;

 public:
  ModifiedJulianDate() = default;

  // Construct julian date object from a single high-precision scalar
  // representation.
  constexpr ModifiedJulianDate(const DoubleDouble& mjd) : BaseClass(mjd) {}

  // Construct julian date object from an unevaluated sum of two scalars.
  constexpr ModifiedJulianDate(const double jd1, const double jd2)
      : BaseClass(DoubleDouble(jd1, jd2)) {}

  using BaseClass::operator<=>;
  using BaseClass::operator==;

  // Get hi and low values of the modified Julian date.
  // The actual modified Julian date is jd1 + jd2.
  constexpr inline auto GetJD1() const -> double { return value_.GetHi(); }
  constexpr inline auto GetJD2() const -> double { return value_.GetLo(); }
};

// Print the modified Julian date to the stream.
// Prints as a value in MJD followed with the format designator "MJD".
inline auto operator<<(std::ostream& os, const ModifiedJulianDate& mjd)
    -> std::ostream& {
  os << DoubleDouble(mjd) << " MJD";
  return os;
}

// Conversion to and from the internal time representation which stores time as
// a high-precision DoubleDouble Julian date value.
template <>
struct TimeFormatTraits<ModifiedJulianDate> {
  static constexpr auto ToJulianDate(const ModifiedJulianDate& mjd)
      -> DoubleDouble {
    return DoubleDouble(mjd) + constants::kJulianDateEpochMJD;
  }

  static constexpr auto FromJulianDate(const DoubleDouble& jd)
      -> ModifiedJulianDate {
    return ModifiedJulianDate(jd - constants::kJulianDateEpochMJD);
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
