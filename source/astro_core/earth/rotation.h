// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [IERS2010] Gerard Petit, and Brian Luzum, IERS Conventions (2010).

#pragma once

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Calculate Earth rotation angle using IAU 2000 model th the given Julian Date
// in UT1 time scale.
//
// The result is in radians, [0, 2*pi) range.
//
// This is an equivalent of eraEra00's eraSp00().
inline auto EarthRotationAngle(const JulianDate& jd_ut1) -> double {
  // [IERS2010] Page 52, Eq. (5.15).
  //
  // Implements the fractional approach to match precision with ERFA more
  // closely. Using double-double calculations lead to slightly different
  // result.

  const DoubleDouble Tu =
      DoubleDouble(jd_ut1 - constants::kJulianDateEpochJ2000);

  const double t = double(Tu);
  const double f = Modulo(Tu.GetLo(), 1.0) + Modulo(Tu.GetHi(), 1.0);

  return NormalizeRadians(2 * constants::pi *
                          (f + 0.7790572732640 + 0.00273781191135448 * t));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
