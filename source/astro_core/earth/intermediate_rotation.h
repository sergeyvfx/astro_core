// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

// Various rotation matrices for intermediate coordinate transforms, which do
// belong to a single type of parameterization. For example, NPB matrices.

#pragma once

#include "astro_core/numeric/numeric.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class JulianDate;

// Construct a precession-nutation rotation matrix (including the frame bias)
// for the given time point. Uses IAU 2006 precession and IAU 2000A nutation
// models.
//
// The time is provided in Julian Date format, TT scale.
//
// This is an equivalent of ERFA's eraPnm06a().
// NOTE: ERFA uses slightly different parameterization, so the result is not
// exactly the same.
auto BiasPrecessionNutationRotation06A(const JulianDate& jd_tt) -> Mat3;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
