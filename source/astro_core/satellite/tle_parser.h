// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Two-line element set (TLE)
//
// This is a data format encoding a list of orbital elements of an
// Earth-orbiting object for a given point in time. The TLE data representation
// is specific to the simplified perturbations models (SGP, SGP4, SDP4, SGP8 and
// SDP8).
//
// References:
//
//   NORAD Two-Line Element Set Format
//   https://celestrak.org/NORAD/documentation/tle-fmt.php

#pragma once

#include <string_view>

#include "astro_core/base/result.h"
#include "astro_core/satellite/tle.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Parser of TLE data from its string representation.
class TLEParser {
 public:
  enum class Error {
    // Generic parse error.
    kError,

    // There is no enough data in the provided TLE lines.
    // For example, one of the lines is too short.
    kInputTruncated,

    // Detected error in the format of the TLE lines.
    // For example, the line number stored in the string did not mmatch the
    // actual line number.
    kInvalidFormat,
  };

  using Result = astro_core::Result<TLE, Error>;

  // Parse TLE from the given lines.
  static auto FromLines(std::string_view line1, std::string_view line2)
      -> Result;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
