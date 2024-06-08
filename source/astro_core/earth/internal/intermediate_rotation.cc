// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [Wallace2006] Precession-nutation procedures consistent with IAU 2006
//     resolutions, P. T. Wallace and N. Capitaine
//     Astronomy and Astrophysics, 459 3 (2006) 981-985
//     DOI: https://doi.org/10.1051/0004-6361:20065897

#include "astro_core/earth/intermediate_rotation.h"

#include "astro_core/earth/nutation.h"
#include "astro_core/earth/precession.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto BiasPrecessionNutationRotation06A(const JulianDate& jd_tt) -> Mat3 {
  // [Wallace2006], page 983, eq. (4).
  const PrecessionAngles06 precession_angles06 =
      CalculatePrecessionAngles06(jd_tt);

  // [Wallace2006], page 983, eq. (5).
  const Nutation nutation = CalculateNutation06A(jd_tt);

  // [Wallace2006], page 983, eq. (6).
  PrecessionAngles06 precession_angles = precession_angles06;
  precession_angles.psib += nutation.dpsi;
  precession_angles.epsa += nutation.deps;

  // [Wallace2006], page 983, eq. (7).
  return PrecessionRotation(precession_angles);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
