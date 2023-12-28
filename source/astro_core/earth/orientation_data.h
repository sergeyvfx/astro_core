// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Processed data which provdes access to earth orientation parameters in a
// thread-safe manner.
//
// The units in the data are closely following the units provided in IERS
// tables.

#pragma once

#include <memory>

#include "astro_core/numeric/numeric.h"
#include "astro_core/table/shared_table.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ModifiedJulianDate;
class EarthOrientationTable;

class EarthOrientationData {
 public:
  using Table = EarthOrientationTable;

  EarthOrientationData();
  ~EarthOrientationData();

  // Set table which provides the earth orientation parameters.
  // The current data is fully replaced with the new one.
  void SetTable(Table&& table);

  // TODO(sergey): Provide API to combine multiple tables to improve coverage
  // and accuracy. For example, to allow merging IERS A and IERS B.

  // Lookup UT1-UTC in seconds for the given time in UTC scale provided in the
  // modified Julian date (MJD) format.
  // If the table has not been provided then 0 is returned.
  auto LookupUT1MinusUTCSecondsInUTCScale(const ModifiedJulianDate& mjd) const
      -> double;

  // Lookup polar motion in arcsec for the given time in UTC scale provided in
  // the modified Julian date (MJD) format.
  // If the table has not been provided then (0, 0) is returned.
  auto LookupPolarMotionArcsecInUTCScale(const ModifiedJulianDate& mjd) const
      -> Vec2;

 private:
  // Table with known constant corrections. It is typically provided by IERS.
  using SharedTable = astro_core::SharedTable<Table>;
  SharedTable shared_table_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
