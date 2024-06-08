// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <iosfwd>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class JulianDate;

// Luni-solar + planetary nutation parameters.
struct Nutation {
  double dpsi = 0;  // Nutation in longitude ∆ψ, radians.
  double deps = 0;  // Nutation in obliquity ∆ε, radians.
};
auto operator<<(std::ostream& os, const Nutation& nutation) -> std::ostream&;

// Calculate nutation using IAU 2000A model.
// The time is provided in Julian Date format, TT scale.
//
// This is an equivalent of ERFA's eraNut00a().
// NOTE: ERFA uses slightly different parameterization, so the result is not
// exactly the same.
auto CalculateNutation00A(const JulianDate& jd_tt) -> Nutation;

// Calculate IAU 2000A nutation with adjustments to match the IAU 2006
// precession.
// The time is provided in Julian Date format, TT scale.
//
// This is an equivalent of ERFA's eraNut06a().
// NOTE: ERFA uses slightly different parameterization, so the result is not
// exactly the same.
auto CalculateNutation06A(const JulianDate& jd_tt) -> Nutation;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
