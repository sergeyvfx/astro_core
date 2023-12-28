// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// A traits-like API for time points comparison.
//
// The comparison of time points might be non-trivial when the time points are
// measured in different scales. This is main reason to implement the comparison
// as an explicit function calls as opposite of overriding comparison
// operations.

#pragma once

#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

struct TimeCompare {
  static auto lt(const Time& lhs, const Time& rhs) -> bool {
    return lhs.AsFormat<JulianDate>() <
           rhs.ToScale(lhs.GetScale()).AsFormat<JulianDate>();
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
