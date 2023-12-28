// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [Meeus1998] Meeus, Jean. Astronomical Algorithms. 2nd ed.,
//     Willmann-Bell, Inc., 1998.
//
//   [Simon1994] Simon, J.L. and Bretagnon, P. (1994) Numerical Expression for
//     Formulae & Mean Elements for the Moon & Planets. Astronomy and
//     Astrophysics, 282, 663-683.
//
//   [Hilton2006] Hilton, L.L. et al. (2006) Report of the international
//     astronomical union division I working group on precession and the
//     ecliptic. Celestial Mechanics and Dynamical Astronomy, 94, 351.
//
//   [Vallado2013] David A Vallado and Wayne D Macclain. 2013.
//       Fundamentals of astrodynamics and applications.

#pragma once

#include "astro_core/coordinate/gcrf.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Coordinate of a moon as per the [Meeus1998] Moon position calculation
// algorithm.
struct MeeusMoonCoordinate {
  // Longitude, radians.
  double lambda = 0;

  // Latitude, radians.
  double beta = 0;

  // Distance, meters.
  double delta = 0;
};

// Calculate Earth's Moon position at the given time using Meeus algorithm
// described in [Meeus1998].
// The result is in mean ecliptic coordinates of date.
auto GetMeeusMoonCoordinate(const Time& time) -> MeeusMoonCoordinate;

// Calculate Earth's Moon position at the given time in GCRF coordinate frame.
auto GetMoonCoordinate(const Time& time) -> GCRF;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
