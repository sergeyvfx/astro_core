// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// Lower-level common building blocks to form coordinate transform matrices.
//
// References:
//
//   [Vallado2013] Vallado, David A., and James Wertz.
//     Fundamentals of Astrodynamics and Applications. 2013.
//
//   [IERS2010] Gerard Petit, and Brian Luzum, IERS Conventions (2010).

#pragma once

#include "astro_core/base/double_double.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// The TIO locator s', positioning the Terrestrial Intermediate Origin
// on the equator of the Celestial Intermediate Pole.
//
// Returns the TIO locator s' in radians.
//
// This is an equivalent of ERFA's eraSp00().
inline auto TerrestrialIntermediateOriginLocator(const JulianDate& jd_tt)
    -> double {
  // [Vallado2013] Page 212, Eq. (3-61).
  // [IERS2010] Page 53, Eq. (5.13).
  //
  // The paper do not seem to specify the scale of t. The t conversion follows
  // the implementation in ERFA sp00.

  const DoubleDouble t =
      DoubleDouble((jd_tt - constants::kJulianDateEpochJ2000) /
                   constants::kNumDaysInJulianCentury);

  return double(ArcsecToRadians(-4.7e-5 * t));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
