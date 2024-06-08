// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [Wallace2006] Precession-nutation procedures consistent with IAU 2006
//     resolutions, P. T. Wallace and N. Capitaine
//     Astronomy and Astrophysics, 459 3 (2006) 981-985
//     DOI: https://doi.org/10.1051/0004-6361:20065897

#include "astro_core/earth/nutation.h"

#include <ostream>

#include "astro_core/earth/internal/iers/tab5.3a.h"
#include "astro_core/earth/internal/iers/tab5.3b.h"

#include "astro_core/base/constants.h"
#include "astro_core/base/reverse_view.h"
#include "astro_core/earth/internal/nutation_arguments.h"
#include "astro_core/math/math.h"
#include "astro_core/time/format/julian_date.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto operator<<(std::ostream& os, const Nutation& nutation) -> std::ostream& {
  os << "(dpsi: " << nutation.dpsi << ", deps: " << nutation.deps << ")";
  return os;
}

namespace {

// Calculate argument for the given row of nutation table.
// It is referred to as ARGUMENT in [IERS2010] Page 62, Eq. (5.35).
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

// Common logic of processing tables 5.3a and 5.3b.
// The result is in radians.
template <class Table>
auto CalculateTableRows00a(
    const Table& table,
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  double dpsi = 0;
  for (int j = 0; j < table.size(); ++j) {
    const double tn = Pow(t, j);
    // Reverse order to handle small values first.
    for (const auto& row : reverse_view(table[j])) {
      const double argument = CalculateArgument(
          row, lunisolar_nutation_arguments, planetary_nutation_arguments);
      dpsi +=
          (row.Arg_i_sin * Sin(argument) + row.Arg_i_cos * Cos(argument)) * tn;
    }
  }
  // The table rows are in microarcseconds, convert it to radians.
  return ArcsecToRadians(dpsi / 1000000.0);
}

// Calculate Nutation in longitude ∆ψ (dpsi).
// [IERS2010] Page 62, Eq. (5.35).
auto CalculateLongitudeNutation00a(
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  return CalculateTableRows00a(iers::table::Table53a,
                               t,
                               lunisolar_nutation_arguments,
                               planetary_nutation_arguments);
}

// Calculate Nutation in obliquity ∆ε (deps).
// [IERS2010] Page 62, Eq. (5.35).
auto CalculateObliquityNutation00a(
    const double t,
    const LunisolarNutationArguments& lunisolar_nutation_arguments,
    const PlanetaryNutationArguments& planetary_nutation_arguments) -> double {
  return CalculateTableRows00a(iers::table::Table53b,
                               t,
                               lunisolar_nutation_arguments,
                               planetary_nutation_arguments);
}

}  // namespace

auto CalculateNutation00A(const JulianDate& jd_tt) -> Nutation {
  // [IERS2010] Page 45, Eq. (5.2).
  const double t = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  const LunisolarNutationArguments lunisolar_nutation_arguments =
      CalculateLunisolarNutationArguments(t);
  const PlanetaryNutationArguments planetary_nutation_arguments =
      CalculatePlanetaryNutationArguments(t);

  return {
      .dpsi = CalculateLongitudeNutation00a(
          t, lunisolar_nutation_arguments, planetary_nutation_arguments),
      .deps = CalculateObliquityNutation00a(
          t, lunisolar_nutation_arguments, planetary_nutation_arguments),
  };
}

auto CalculateNutation06A(const JulianDate& jd_tt) -> Nutation {
  // t is the time interval since J2000 in Julian centuries (TT).
  const double t = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  const Nutation nutation00a = CalculateNutation00A(jd_tt);

  // [Wallace2006], page 983, eq. (5).
  Nutation nutation06a;
  const double f = -2.7774e-6 * t;
  nutation06a.dpsi = nutation00a.dpsi + (0.4697e-6 + f) * nutation00a.dpsi;
  nutation06a.deps = nutation00a.deps + f * nutation00a.deps;

  return nutation06a;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
