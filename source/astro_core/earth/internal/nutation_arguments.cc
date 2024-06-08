// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/internal/nutation_arguments.h"

#include "astro_core/math/math.h"
#include "astro_core/numeric/polynomial.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

////////////////////////////////////////////////////////////////////////////////
// Arguments of lunisolar nutation.

// Polynomial calculation for the arguments of lunisolar nutation.
//
// The arguments are expected to be from the [IERS2010] Page 67, Eq. (5.43) in
// arcseconds. The result is in radians.
template <class... Args>
auto LunisolarNutationPolynomial(const double t, Args... args) -> double {
  return ArcsecToRadians(ReduceArcsec(Polynomial(t, args...)));
}

// Calculate the arguments of lunisolar nutation.
// [IERS2010] Page 67, Eq. (5.43).
auto CalculateLunisolarNutationArguments(const double t)
    -> LunisolarNutationArguments {
  LunisolarNutationArguments args;

  // Mean Anomaly of the Moon
  args.l = LunisolarNutationPolynomial(
      t, 134.96340251 * 3600, 1717915923.2178, 31.8792, 0.051635, -0.00024470);

  // Mean Anomaly of the Sun.
  args.l_prime = LunisolarNutationPolynomial(
      t, 357.52910918 * 3600, 129596581.0481, -0.5532, 0.000136, -0.00001149);

  // Mean longitude of the Moon minus that of the ascending node.
  args.F = LunisolarNutationPolynomial(
      t, 93.27209062 * 3600, 1739527262.8478, -12.7512, -0.001037, 0.00000417);

  // Mean Elongation of the Moon from the Sun.
  args.D = LunisolarNutationPolynomial(
      t, 297.85019547 * 3600, 1602961601.2090, -6.3706, 0.006593, -0.00003169);

  // Mean Longitude of the Ascending Node of the Moon.
  args.Om = LunisolarNutationPolynomial(
      t, 125.04455501 * 3600, -6962890.5431, 7.4722, 0.007702, -0.00005939);

  return args;
}

////////////////////////////////////////////////////////////////////////////////
// Arguments for the planetary nutation.

// Calculate the arguments for the planetary nutation.
// [IERS2010] Page 68, Eq. (5.44).
auto CalculatePlanetaryNutationArguments(const double t)
    -> PlanetaryNutationArguments {
  PlanetaryNutationArguments args;

  // Planetary longitudes, Mercury through Neptune.
  args.L_Me = ReduceRadians(4.402608842 + 2608.7903141574 * t);
  args.L_Ve = ReduceRadians(3.176146697 + 1021.3285546211 * t);
  args.L_E = ReduceRadians(1.753470314 + 628.3075849991 * t);
  args.L_Ma = ReduceRadians(6.203480913 + 334.0612426700 * t);
  args.L_J = ReduceRadians(0.599546497 + 52.9690962641 * t);
  args.L_Sa = ReduceRadians(0.874016757 + 21.3299104960 * t);
  args.L_U = ReduceRadians(5.481293872 + 7.4781598567 * t);
  args.L_Ne = ReduceRadians(5.311886287 + 3.8133035638 * t);

  // General accumulated precession in longitude.
  args.p_A = ReduceRadians(0.02438175 * t + 0.00000538691 * t * t);

  return args;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
