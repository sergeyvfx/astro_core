// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [Wallace2006] Precession-nutation procedures consistent with IAU 2006
//     resolutions, P. T. Wallace and N. Capitaine
//     Astronomy and Astrophysics, 459 3 (2006) 981-985
//     DOI: https://doi.org/10.1051/0004-6361:20065897
//
//   [Vallado2013] David A Vallado and Wayne D Macclain. 2013.
//       Fundamentals of astrodynamics and applications.

#include "astro_core/earth/precession.h"

#include "astro_core/base/constants.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/polynomial.h"
#include "astro_core/time/format/julian_date.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto operator<<(std::ostream& os,
                const PrecessionAngles06& precession_angles) -> std::ostream& {
  os << "(gamb: " << precession_angles.gamb
     << ", phib: " << precession_angles.phib
     << ", psib: " << precession_angles.psib
     << ", epsa: " << precession_angles.epsa << ")";
  return os;
}

auto CalculatePrecessionAngles06(const JulianDate& jd_tt)
    -> PrecessionAngles06 {
  // t is the time interval since J2000 in Julian centuries (TT).
  const double t = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  // [Wallace2006], page 983, eq. (4).
  // [Vallado2013], page 218, Eq. (3-74).

  PrecessionAngles06 precession_angles;

  precession_angles.gamb = ArcsecToRadians(Polynomial(t,
                                                      -0.052928,
                                                      10.556378,
                                                      0.4932044,
                                                      -0.00031238,
                                                      -0.000002788,
                                                      0.0000000260));

  precession_angles.phib = ArcsecToRadians(Polynomial(t,
                                                      84381.412819,
                                                      -46.811016,
                                                      0.0511268,
                                                      0.00053289,
                                                      -0.000000440,
                                                      -0.0000000176));

  precession_angles.psib = ArcsecToRadians(Polynomial(t,
                                                      -0.041775,
                                                      5038.481484,
                                                      1.5584175,
                                                      -0.00018522,
                                                      -0.000026452,
                                                      -0.0000000148));

  precession_angles.epsa = ArcsecToRadians(Polynomial(t,
                                                      84381.406,
                                                      -46.836769,
                                                      -0.0001831,
                                                      0.00200340,
                                                      -0.000000576,
                                                      -0.00000004348));

  return precession_angles;
}

auto PrecessionRotation(const PrecessionAngles06& angles) -> Mat3 {
  // [Wallace2006], page 983, eq.(7).
  return ROT1(-angles.epsa) * ROT3(-angles.psib) * ROT1(angles.phib) *
         ROT3(angles.gamb);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
