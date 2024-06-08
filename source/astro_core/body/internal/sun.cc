// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/body/sun.h"

#include "astro_core/base/constants.h"
#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/spherical.h"
#include "astro_core/earth/intermediate_rotation.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/time/format/julian_date.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto GetApproximateSunCoordinate(const Time& time) -> GCRF {
  const Time time_tt = time.ToScale(TimeScale::kTT);
  const JulianDate jd_tt = time_tt.AsFormat<JulianDate>();

  // First, compute D, the number of days and fraction (+ or –) from the epoch
  // referred to as "J2000.0", which is 2000 January 1.5, Julian date 2451545.0:
  const double D = double(jd_tt - constants::kJulianDateEpochJ2000);

  // Mean anomaly of the Sun.
  const double g_deg = 357.529 + 0.98560028 * D;
  const double g_rad = DegreesToRadians(g_deg);

  // Mean longitude of the Sun.
  const double q_deg = 280.459 + 0.98564736 * D;

  // Geocentric apparent ecliptic longitude of the Sun (adjusted for
  // aberration):
  const double L_deg = q_deg + 1.915 * Sin(g_rad) + 0.020 * Sin(2 * g_rad);
  const double L_rad = DegreesToRadians(L_deg);

  // The Sun's ecliptic latitude, b, can be approximated by b=0.

  // The distance of the Sun from the Earth, R, in astronomical units (AU):
  const double R_AU = 1.00014 - 0.01671 * Cos(g_rad) - 0.00014 * Cos(2 * g_rad);

  // The mean obliquity of the ecliptic.
  const double e_deg = 23.439 - 0.00000036 * D;
  const double e_rad = DegreesToRadians(e_deg);

  // Then the Sun's right ascension, RA.
  //
  // RA is always in the same quadrant as L. If the numerator and denominator on
  // the right side of the expression for RA are used in a double-argument
  // arctangent function (e.g., "atan2"), the proper quadrant will be obtained.
  const double RA_rad = ArcTan2(Cos(e_rad) * Sin(L_rad), Cos(L_rad));

  // The Sun's declination, d.
  const double sin_d = Sin(e_rad) * Sin(L_rad);
  const double d_rad = ArcSin(sin_d);

  // Convert equatorial coordinates to GCRF.
  //
  // The algorithm does not explicitly mention which exactly equatorial
  // coordinates it uses, but empirically comparing results of this algorithm
  // with Astrop's `coordinates.get_sun()` it seems to be in the TETE frame.
  // At least, treating them as TETE, converting to GSRF and comparing against
  // the Astropy implementation the error is within 60 arcseconds, which seems
  // to be in agreement with the claimed accuracy of the algorithm/
  //
  // The code below is a simplified version tete_to_gcrs) from Astropy, omitting
  // the parts of TETE->GCRS conversion related on an observer location and
  // velocity.

  const Spherical tete_spherical(
      {.latitude = d_rad, .longitude = RA_rad, .distance = R_AU});
  const Cartesian tete_cartesian = tete_spherical.ToCartesian();

  const Mat3 rbpn = BiasPrecessionNutationRotation06A(jd_tt);
  const Cartesian gsrf_cartesian =
      rbpn.Transposed() * Vec3(tete_cartesian) * constants::kAstronomicalUnit;

  return GCRF({.observation_time = time, .position = gsrf_cartesian});
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
