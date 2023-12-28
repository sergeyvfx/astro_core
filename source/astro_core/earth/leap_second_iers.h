// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parser of the the leap second announcement from the International Earth
// Rotation and Reference Systems Service (IERS) [1].
//
// The leap seconds are announced in the Bulletin C [2]. It is published (sent
// via email) two times a year:
//
//  - In the beginning of January to announce whether there will or not be a
//    leap second introduced on 30 June at midnight.
//
//  - In the beginning of July to announce whether there will or not be a
//    leap second introduced on 31 December at midnight.
//
// The Bulletin C provided by the IERS is an announcement of the leap seconds,
// which is not very convenient for automated parsing. The Paris Observatory
// IERS Centers [3] maintains the machine readable historical track of the leap
// seconds [4] in the form of a table.
//
// NOTE: The leap table is only valid starting January 1, 1972. Since this date
//       only integer number of leap seconds are announced. Prior to this date,
//       however, a fractional leap second was used, which was interpolated
//       throughout the year with occasional announcement of a bigger
//       correction.
//
// NOTE: The Bulletin C and the leap seconds table Leap_Second.dat are valid for
//       almost a year. The data published in the beginning of January is valid
//       until 28 December, an the data published in the beginning of July is
//       28 June.
//
// References:
//
//   [1] International Earth Rotation and Reference Systems Service
//       https://www.iers.org/IERS/EN/Home/home_node.html
//
//   [2] Bulletin C - Product metadata
//       https://datacenter.iers.org/productMetadata.php?id=16
//       https://www.iers.org/IERS/EN/DataProducts/EarthOrientationData/eop.html
//
//   [3] Paris Observatory IERS Centers
//       https://hpiers.obspm.fr/
//
//   [4] Leap_Second.dat
//       https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat

#pragma once

#include <string_view>

#include "astro_core/base/result.h"
#include "astro_core/earth/leap_second_table.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Parser of the historical track of leap second provided by the Paris
// Observatory IERS Centers (Leap_Second.dat [4]).
class LeapSecondIERS {
 public:
  enum class Error {
    // The table was unable to be parsed, possibly due to the wrong format or
    // a corrupted input.
    kError,
  };

  using Result = astro_core::Result<LeapSecondTable, Error>;

  // Parse table provided in the text form: each table row is expected to be
  // in its own line.
  static auto Parse(std::string_view table_text) -> Result;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
