// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Utilities to convert time to be relative to various epoch.

#pragma once

#include "astro_core/base/constants.h"
#include "astro_core/base/double_double.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

constexpr inline auto JulianCenturiesSinceJ2000(const JulianDate& jd)
    -> DoubleDouble {
  return ((DoubleDouble{jd} - constants::kJulianDateEpochJ2000) /
          constants::kNumDaysInJulianYear) /
         100.0;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
