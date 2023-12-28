// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Utilities to calculate the Greenwich sidereal time in different models.
//
// References:
//
//   [Vallado2006] Vallado, David A., Paul Crawford, Richard Hujsak, and T.S.
//       Kelso, "Revisiting Spacetrack Report #3," presented at the AIAA/AAS
//       Astrodynamics Specialist Conference, Keystone, CO, 2006 August 21–24.
//
//   [Vallado2013] David A Vallado and Wayne D Macclain. 2013.
//       Fundamentals of astrodynamics and applications.

#pragma once

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/math/math.h"
#include "astro_core/time/epoch_convert.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Calculate the Greenwich mean sidereal time from Universal Time using IAU 1982
// model.
//
// The universal time is provided as a Julian Date.
//
// Returns the the Greenwich mean sidereal time in radians, [0 .. 2*pi].
//
// This is an equivalent of ERFA's eraGmst82().
inline auto GreenwichMeanSiderealTime1982(const JulianDate& jd_ut1)
    -> DoubleDouble {
  // The implementation follows the algorithm from [Vallado2013] Example 3-5.
  // The same algorithm is implemented in the [Vallado2006] page 87.

  // Julian centuries since the J2000 epoch. The tut1 follows the naming of
  // Tut1.
  const DoubleDouble tut1 = JulianCenturiesSinceJ2000(jd_ut1);

  const DoubleDouble tut1_2 = tut1 * tut1;    // Tut1^2
  const DoubleDouble tut1_3 = tut1_2 * tut1;  // Tut1^3

  // [Vallado2013] Eq. (3-47).
  const DoubleDouble gmst_seconds =
      67310.54841 + (DoubleDouble(876600.0) * 3600.0 + 8640184.812866) * tut1 +
      0.093104 * tut1_2 - 6.2e-6 * tut1_3;

  // The original algorithm consists of the following steps:
  //
  //   - Reduce gmst_seconds to a result within the range of 86400 seconds
  //   - Then convert to degrees by dividing by 240 (1 second = 1/240 degrees
  //     using [Vallado2013] Eq. (3-37)).
  //
  // We convert to degrees by dividing by 240, convert to radians, and wrap into
  // intervals of 2*pi.
  DoubleDouble gmst = Modulo(DegreesToRadians(gmst_seconds / 240.0),
                             2 * DoubleDouble(constants::pi));

  // Make sure the result is positive.
  if (gmst < 0.0) {
    gmst += 2 * constants::pi;
  }

  return gmst;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
