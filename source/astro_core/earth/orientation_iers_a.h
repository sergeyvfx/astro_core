// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parser of an earth orientation parameters provided by the IERS BULLETIN A
// [1]. This bulletin contains Earth orientation parameters, UT1-UTC, their
// errors at daily intervals, and predictions for 1 year into the future.
//
// The table with information can be downloaded from the IERS site [2].
//
// References:
//
//   [1] BULLETIN A - Product metadata
//       https://datacenter.iers.org/productMetadata.php?id=6
//       https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop.html
//
//   [2] finals.all (IAU2000)
//       https://datacenter.iers.org/data/latestVersion/finals.all.iau2000.txt

#pragma once

#include <string_view>

#include "astro_core/base/result.h"
#include "astro_core/earth/orientation_table.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Parser of the historical track of the earth orientation parameters provided
// by the IERS (finals.all [2]).
class EarthOrientationIERSA {
 public:
  enum class Error {
    // The table was unable to be parsed, possibly due to the wrong format or
    // a corrupted input.
    kError,
  };

  using Result = astro_core::Result<EarthOrientationTable, Error>;

  // Parse table provided in the text form: each table row is expected to be
  // in its own line.
  static auto Parse(std::string_view table_text) -> Result;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
