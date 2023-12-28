// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second_table.h"

#include <span>

#include "astro_core/base/constants.h"
#include "astro_core/math/math.h"
#include "astro_core/table/lookup.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

void LeapSecondTable::Preprocess() {
  // Sort the rows, enabling binary search in the rows.
  std::sort(
      table_.begin(), table_.end(), [](const Row& a, const Row& b) -> bool {
        return a.mjd_utc < b.mjd_utc;
      });

  // Initialize values of the MJD in TAI time scale.
  for (Row& row : table_) {
    row.mjd_tai =
        ModifiedJulianDate(DoubleDouble(row.mjd_utc) +
                           row.tai_minus_utc / constants::kNumSecondsInDay);
  }
}

auto LeapSecondTable::LookupTAIMinusUTCSecondsInUTCScale(
    const ModifiedJulianDate& mjd_utc) const -> double {
  if (table_.empty()) {
    return 0;
  }

  const Row& row = LookupFloorRow<&Row::mjd_utc>(table_, mjd_utc);

  if (&row == &table_.back()) {
    // The row is at the end ot table, no further check is needed.
    return row.tai_minus_utc;
  }

  const Row& next_row = *(&row + 1);

  const ModifiedJulianDate mjd_utc_begin_of_last_day(next_row.mjd_utc - 1.0);

  if (mjd_utc <= mjd_utc_begin_of_last_day) {
    // The date is prior to the day at which the next leap second is introduced.
    // Can use the TAI-UTC value from the row.
    return row.tai_minus_utc;
  }

  // Smear the leap second on the day before a new leap second introduction.
  // This avoids ambiguity between TAI and UTC scales at the leap second.
  // This matches AstroPy and Standards of Fundamental Astronomy (SOFA).
  //
  // Additional source:
  //   https://github.com/astropy/astropy/issues/10167
  const DoubleDouble weight{mjd_utc - mjd_utc_begin_of_last_day};
  return double(Lerp(row.tai_minus_utc, next_row.tai_minus_utc, weight));
}

auto LeapSecondTable::LookupTAIMinusUTCSecondsInTAIScale(
    const ModifiedJulianDate& mjd_tai) const -> double {
  if (table_.empty()) {
    return 0;
  }

  const Row& row = LookupFloorRow<&Row::mjd_tai>(table_, mjd_tai);

  if (&row == &table_.back()) {
    // The row is at the end ot table, no further check is needed.
    return row.tai_minus_utc;
  }

  const Row& next_row = *(&row + 1);

  const ModifiedJulianDate mjd_tai_begin_of_last_day(
      next_row.mjd_utc - 1.0 +
      DoubleDouble(row.tai_minus_utc) / constants::kNumSecondsInDay);

  if (mjd_tai <= mjd_tai_begin_of_last_day) {
    // The date is prior to the day at which the next leap second is introduced.
    // Can use the TAI-UTC value from the row.
    return row.tai_minus_utc;
  }

  // Smear the leap second on the day before a new leap second introduction.
  // This avoids ambiguity between TAI and UTC scales at the leap second.
  // This matches AstroPy and Standards of Fundamental Astronomy (SOFA).
  const DoubleDouble weight =
      DoubleDouble((mjd_tai - mjd_tai_begin_of_last_day) /
                   (next_row.mjd_tai - mjd_tai_begin_of_last_day));
  return double(Lerp(row.tai_minus_utc, next_row.tai_minus_utc, weight));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
