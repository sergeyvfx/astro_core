// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

// The expressions for the fundamental arguments of nutation.
//
// References:
//
//   [IERS2010] Gerard Petit, and Brian Luzum, IERS Conventions (2010).

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

/////////////////////////////////////////////////////////////////////////////////
// Arguments of lunisolar nutation.
//
// Follows parameterization and equations from [IERS2010] Page 67, Eq. (5.43).
//
// The parameter t is defined by ([IERS2010] Page 45, Eq. (5.2)):
//   t = (TT − 2000 January 1d 12h TT) in days/36525

struct LunisolarNutationArguments {
  double l;        // Mean Anomaly of the Moon, eraFal03().
  double l_prime;  // l', Mean Anomaly of the Sun, eraFalp03().
  double F;   // Mean longitude of the Moon minus that of the ascending node.
              // eraFaf03().
  double D;   // Mean Elongation of the Moon from the Sun, eraFad03().
  double Om;  // Mean Longitude of the Ascending Node of the Moon, eraFaom03().
};

// Calculate the arguments of lunisolar nutation.
auto CalculateLunisolarNutationArguments(double t)
    -> LunisolarNutationArguments;

////////////////////////////////////////////////////////////////////////////////
// Arguments for the planetary nutation.
//
// Follows parameterization and equations from [IERS2010] Page 68, Eq. (5.44).
//
// The parameter t is defined by ([IERS2010] Page 45, Eq. (5.2)):
//   t = (TT − 2000 January 1d 12h TT) in days/36525

struct PlanetaryNutationArguments {
  // Planetary mean longitudes, Mercury through Neptune, and their ERFA's
  // analogues.
  double L_Me;  // Mercury, eraFame03().
  double L_Ve;  // Venus, eraFave03().
  double L_E;   // Earth, eraFae03().
  double L_Ma;  // Mars, eraFama03().
  double L_J;   // Jupiter, eraFaju03().
  double L_Sa;  // Saturn, eraFasa03().
  double L_U;   // Uranus, eraFaur03().
  double L_Ne;  // Neptune, eraFane03().

  // General accumulated precession in longitude.
  // ERFA's eraFapa03().
  double p_A;
};

// Calculate the arguments for the planetary nutation.
auto CalculatePlanetaryNutationArguments(double t)
    -> PlanetaryNutationArguments;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
