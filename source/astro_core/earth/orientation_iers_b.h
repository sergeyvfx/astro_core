// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parser of an earth orientation parameters provided by the IERS BULLETIN B
// [1]. This bulletin contains Earth orientation parameters, UT1-UTC, their
// errors at daily intervals.
//
// The table with information can be downloaded from the IERS site [2]. The
// parent page link is [3].
//
// References:
//
//   [1] BULLETIN B - Product metadata
//       https://datacenter.iers.org/productMetadata.php?id=207
//       https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop.html
//
//   [2] eopc04_IAU2000.62-now
//       https://hpiers.obspm.fr/iers/eop/eopc04/eopc04_IAU2000.62-now
//
//   [3] EOP C04 COMBINED SERIES
//       https://hpiers.obspm.fr/eop-pc/index.php?index=C04&lang=en

#pragma once

#include <string_view>

#include "astro_core/base/result.h"
#include "astro_core/earth/orientation_table.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Parser of the historical track of the EOP C04 combined series [2].
class EarthOrientationIERSB {
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
