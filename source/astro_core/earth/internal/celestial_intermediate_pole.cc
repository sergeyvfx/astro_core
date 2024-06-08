// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [IERS2010] Gerard Petit, and Brian Luzum, IERS Conventions (2010).

#include "astro_core/earth/celestial_intermediate_pole.h"

#include "astro_core/earth/internal/iers/tab5.2a.h"
#include "astro_core/earth/internal/iers/tab5.2b.h"
#include "astro_core/earth/internal/iers/tab5.2d.h"
#include "astro_core/earth/internal/nutation_arguments.h"

#include "astro_core/base/constants.h"
#include "astro_core/base/reverse_view.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/polynomial.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

////////////////////////////////////////////////////////////////////////////////
// X, Y coordinates of celestial intermediate pole.

namespace {

// Calculate polynomial part of the coordinates.
// The result in arcseconds.
//
// [IERS2010] Page 54, Eq. (5.16).
auto CIPPolynomialPart(const double t) -> Vec2 {
  return ArcsecToRadians(Polynomial(t,
                                    Vec2(-0.016617, -0.006951),
                                    Vec2(2004.191898, -0.025896),
                                    Vec2(-0.4297829, -22.4072747),
                                    Vec2(-0.19861834, 0.00190059),
                                    Vec2(0.000007578, 0.001112526),
                                    Vec2(0.0000059285, 0.0000001358)));
}

// Calculate argument for the given row of CIP coordinate non-polynomial part.
// It is referred to as ARGUMENT in [IERS2010] Page 54, Eq. (5.16).
template <class Row>
auto CalculateArgument(
    const Row& row,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  double arg = 0;

  // Lunisolar nutation arguments.
  arg += row.l * lunisolar_nutation_arguments.l;
  arg += row.l_prime * lunisolar_nutation_arguments.l_prime;
  arg += row.F * lunisolar_nutation_arguments.F;
  arg += row.D * lunisolar_nutation_arguments.D;
  arg += row.Om * lunisolar_nutation_arguments.Om;

  // Planetary nutation arguments.
  arg += row.L_Me * planetary_nutation_arguments.L_Me;
  arg += row.L_Ve * planetary_nutation_arguments.L_Ve;
  arg += row.L_E * planetary_nutation_arguments.L_E;
  arg += row.L_Ma * planetary_nutation_arguments.L_Ma;
  arg += row.L_J * planetary_nutation_arguments.L_J;
  arg += row.L_Sa * planetary_nutation_arguments.L_Sa;
  arg += row.L_U * planetary_nutation_arguments.L_U;
  arg += row.L_Ne * planetary_nutation_arguments.L_Ne;
  arg += row.p_A * planetary_nutation_arguments.p_A;

  return arg;
}

// Calculate periodic nutation series for the X coordinate.
// [IERS2010] Page 54, Eq. (5.16).
auto CIPPeriodicNutationTermsForX(
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  using iers::table::Table52a;
  using iers::table::Table52aRow;

  double x = 0;

  for (int j = Table52a.size() - 1; j >= 0; --j) {
    const double tn = Pow(t, j);
    // Reverse order to handle small values first.
    for (const Table52aRow& row : reverse_view(Table52a[j])) {
      const double argument = CalculateArgument(
          row, lunisolar_nutation_arguments, planetary_nutation_arguments);

      x += (row.a_s_j_i * Sin(argument) + row.a_c_j_i * Cos(argument)) * tn;
    }
  }

  // The table rows are in microarcseconds, convert it to radians.
  return ArcsecToRadians(x / 1000000.0);
}

// Calculate periodic nutation series for the Y coordinate.
// [IERS2010] Page 54, Eq. (5.16).
auto CIPPeriodicNutationTermsForY(
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  using iers::table::Table52b;
  using iers::table::Table52bRow;

  double y = 0;

  for (int j = Table52b.size() - 1; j >= 0; --j) {
    const double tn = Pow(t, j);
    // Reverse order to handle small values first.
    for (const Table52bRow& row : reverse_view(Table52b[j])) {
      const double argument = CalculateArgument(
          row, lunisolar_nutation_arguments, planetary_nutation_arguments);

      y += (row.b_c_j_i * Cos(argument) + row.b_s_j_i * Sin(argument)) * tn;
    }
  }

  // The table rows are in microarcseconds, convert it to radians.s
  return ArcsecToRadians(y / 1000000.0);
}

auto CelestialIntermediatePole(
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> Vec2 {
  Vec2 xy = CIPPolynomialPart(t);

  xy(0) += CIPPeriodicNutationTermsForX(
      t, lunisolar_nutation_arguments, planetary_nutation_arguments);
  xy(1) += CIPPeriodicNutationTermsForY(
      t, lunisolar_nutation_arguments, planetary_nutation_arguments);

  return xy;
}

}  // namespace

auto CelestialIntermediatePole(const JulianDate& jd_tt) -> Vec2 {
  // [IERS2010] Page 45, Eq. (5.2).
  const double t = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  const LunisolarNutationArguments lunisolar_nutation_arguments =
      CalculateLunisolarNutationArguments(t);
  const PlanetaryNutationArguments planetary_nutation_arguments =
      CalculatePlanetaryNutationArguments(t);

  return CelestialIntermediatePole(
      t, lunisolar_nutation_arguments, planetary_nutation_arguments);
}

////////////////////////////////////////////////////////////////////////////////
// The CIO locator s, positioning the Celestial Intermediate Origin on the
// equator of the Celestial Intermediate Pole.

namespace {

auto CIOPolynomialPart(const double t) -> double {
  // The equation is given in microarcseconds, the result of this function is in
  // radians.
  //
  // [IERS2010] Page 59, Table 5.2d.
  //
  // The tab5.2d.txt contains an expanded formula:
  //   94.0 + 3808.65 t - 122.68 t^2 - 72574.11 t^3 + 27.98 t^4 + 15.62 t^5

  return ArcsecToRadians(
      Polynomial(t, 94.0, 3808.65, -122.68, -72574.11, 27.98, 15.62) /
      1000000.0);
}

// Calculate periodic nutation series for the CIO locator s.
// [IERS2010] Page 59, Table 5.2d.
auto CIOPeriodicNutationTerms(
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  using iers::table::Table52d;
  using iers::table::Table52dRow;

  double x = 0;

  for (int j = Table52d.size() - 1; j >= 0; --j) {
    const double tn = Pow(t, j);
    // Reverse order to handle small values first.
    for (const Table52dRow& row : reverse_view(Table52d[j])) {
      const double argument = CalculateArgument(
          row, lunisolar_nutation_arguments, planetary_nutation_arguments);

      x += (row.c_s_j_i * Sin(argument) + row.c_c_j_i * Cos(argument)) * tn;
    }
  }

  // The table roes are in microarcseconds, convert it to radians.s
  return ArcsecToRadians(x / 1000000.0);
}

auto CelestialIntermediateOriginLocator(
    const double t,
    const Vec2& cip_xy,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  double s = CIOPolynomialPart(t);

  s += CIOPeriodicNutationTerms(
      t, lunisolar_nutation_arguments, planetary_nutation_arguments);

  s -= cip_xy(0) * cip_xy(1) / 2;

  return s;
}

}  // namespace

auto CelestialIntermediateOriginLocator(const JulianDate& jd_tt,
                                        const Vec2& cip_xy) -> double {
  // [IERS2010] Page 45, Eq. (5.2).
  const double t = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  const LunisolarNutationArguments lunisolar_nutation_arguments =
      CalculateLunisolarNutationArguments(t);
  const PlanetaryNutationArguments planetary_nutation_arguments =
      CalculatePlanetaryNutationArguments(t);

  return CelestialIntermediateOriginLocator(
      t, cip_xy, lunisolar_nutation_arguments, planetary_nutation_arguments);
}

////////////////////////////////////////////////////////////////////////////////
// Celestial to intermediate-frame-of-date matrix.

auto CelestialToIntermediateFrameOfDateMatrix(const Vec2& cip_xy,
                                              const double s) -> Mat3 {
  // Solve the [IERS2010] Page 48, Eq. (5.7) to obtain spherical angles E and d.
  //
  // The CIP coordinates form a unit vector in the Geocentric CelestialReference
  // System (from ERFA).

  const double a2 = cip_xy(0) * cip_xy(0) + cip_xy(1) * cip_xy(1);

  const double E = (a2 > 0.0) ? ArcTan2(cip_xy(1), cip_xy(0)) : 0.0;
  const double d = ArcTan(Sqrt(a2 / (1.0 - a2)));

  // [IERS2010] Page 48, Eq. (5.6).
  return ROT3(-(E + s)) * ROT2(d) * ROT3(E) * Mat3::Identity();
}

auto CelestialToIntermediateFrameOfDateMatrix(const JulianDate& jd_tt) -> Mat3 {
  // [IERS2010] Page 45, Eq. (5.2).
  const double t = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  const LunisolarNutationArguments lunisolar_nutation_arguments =
      CalculateLunisolarNutationArguments(t);
  const PlanetaryNutationArguments planetary_nutation_arguments =
      CalculatePlanetaryNutationArguments(t);

  const Vec2 cip_xy = CelestialIntermediatePole(
      t, lunisolar_nutation_arguments, planetary_nutation_arguments);

  const double s = CelestialIntermediateOriginLocator(
      t, cip_xy, lunisolar_nutation_arguments, planetary_nutation_arguments);

  return CelestialToIntermediateFrameOfDateMatrix(cip_xy, s);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
