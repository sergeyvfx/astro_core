// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Processed data which provdes access to leap second information in a
// thread-safe manner.

#pragma once

#include <memory>

#include "astro_core/table/shared_table.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ModifiedJulianDate;
class LeapSecondTable;

class LeapSecondData {
 public:
  using Table = LeapSecondTable;

  LeapSecondData();
  ~LeapSecondData();

  // Set table which provides TAI-UTC information.
  // The current data is fully replaced with the new one.
  void SetTable(Table&& table);

  // Lookup TAI-UTC value for the given time in MJD format and UTC scale.
  //
  // If the date is prior to the known period of the TAI-UTC 0 is returned.
  // If the table has not been provided then for time range where table is
  // needed zero is returned.
  auto LookupTAIMinusUTCSecondsInUTCScale(
      const ModifiedJulianDate& mjd_utc) const -> double;

  // Lookup TAI-UTC value for the given time in MJD format and TAI scale.
  //
  // If the date is prior to the known period of the TAI-UTC 0 is returned.
  // If the table has not been provided then for time range where table is
  // needed zero is returned.
  auto LookupTAIMinusUTCSecondsInTAIScale(
      const ModifiedJulianDate& mjd_tai) const -> double;

 private:
  // Table with known constant corrections. It is typically provided by IERS.
  using SharedTable = astro_core::SharedTable<Table>;
  SharedTable shared_table_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
