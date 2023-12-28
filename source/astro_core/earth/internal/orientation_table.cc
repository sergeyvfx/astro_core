// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation_table.h"

#include "astro_core/table/lookup.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

void EarthOrientationTable::Preprocess() {
  // Sort the rows, enabling binary search in the rows.
  std::sort(table_.begin(),
            table_.end(),
            [](const Row& a, const Row& b) -> bool { return a.mjd < b.mjd; });
}

auto EarthOrientationTable::LookupUT1MinusUTCSecondsInUTCScale(
    const ModifiedJulianDate& mjd) const -> double {
  if (table_.empty()) {
    return 0;
  }

  // Perform linear interpolation. This is also how it is done in Astropy 5.1
  // (which follows TEMOPO).
  //
  // Possible improvement is to follow [1] for more precise interpolation and
  // correction for tidal effects.
  //
  //   [1] INTERPOLATING IERS EARTH ORIENTATION DATA
  //       Revised version of the IERS Gazette No 13, 29 January 1997
  //       https://hpiers.obspm.fr/iers/models/interp.readme

  return LinearInterpolate<&Row::mjd, &Row::ut1_minus_utc, DoubleDouble>(table_,
                                                                         mjd);
}

auto EarthOrientationTable::LookupPolarMotionArcsecInUTCScale(
    const ModifiedJulianDate& mjd) const -> Vec2 {
  if (table_.empty()) {
    return {0, 0};
  }

  // Perform linear interpolation. This is also how it is done in Astropy 5.1
  // (which follows TEMOPO).
  //
  // Possible improvement is to follow [1] for more precise interpolation and
  // correction for tidal effects.
  //
  //   [1] INTERPOLATING IERS EARTH ORIENTATION DATA
  //       Revised version of the IERS Gazette No 13, 29 January 1997
  //       https://hpiers.obspm.fr/iers/models/interp.readme

  return LinearInterpolate<&Row::mjd, &Row::polar_motion, DoubleDouble>(table_,
                                                                        mjd);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
