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

#include "astro_core/base/constants.h"
#include "astro_core/base/reverse_view.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/polynomial.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

////////////////////////////////////////////////////////////////////////////////
// Arguments of lunisolar nutation.

namespace {

struct LunisolarNutationArguments {
  double l;        // Mean Anomaly of the Moon.
  double l_prime;  // l', Mean Anomaly of the Sun.
  double F;
  double D;   // Mean Elongation of the Moon from the Sun.
  double Om;  //  Mean Longitude of the Ascending Node of the Moon.
};

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

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Arguments for the planetary nutation.

namespace {

struct PlanetaryNutationArguments {
  // Planetary longitudes, Mercury through Neptune.
  double L_Me;
  double L_Ve;
  double L_E;
  double L_Ma;
  double L_J;
  double L_Sa;
  double L_U;
  double L_Ne;

  // General accumulated precession in longitude.
  double p_A;
};

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

}  // namespace

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

  // The table roes are in microarcseconds, convert it to radians.s
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
