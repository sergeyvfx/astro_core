// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <iosfwd>

#include "astro_core/numeric/numeric.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class JulianDate;

// Precession angles, IAU 2006 (Fukushima-Williams 4-angle formulation).
struct PrecessionAngles06 {
  double gamb = 0;  // Angle gamma_bar γ ̄, radians.
  double phib = 0;  // Angle phi_bar φ ̄, radians.
  double psib = 0;  // Angle psi_bar ψ ̄, radians.
  double epsa = 0;  // Angle epsilon_A εA, radians.
};
auto operator<<(std::ostream& os,
                const PrecessionAngles06& precession_angles) -> std::ostream&;

// Calculate precession angles, IAU 2006 (Fukushima-Williams 4-angle
// formulation).
// The time is provided in Julian Date format, TT scale.
//
// This is an equivalent of ERFA's eraPfw06().
auto CalculatePrecessionAngles06(const JulianDate& jd_tt) -> PrecessionAngles06;

// Construct rotation matrix from the given Fukushima-Williams precession
// angles.
//
// This is an equivalent of ERFA's eraFw2m().
auto PrecessionRotation(const PrecessionAngles06& angles) -> Mat3;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
