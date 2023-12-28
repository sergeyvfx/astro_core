// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Earth orientation table which specifies parameters like UT1-UTC and polar
// motion.
//
// The table defines the run-time format which is needed for its operation and
// methods to obtain the Earth orientation parameters for the given time point.
// It does not define the format from which the data for it is obtained.
//
// The lookup access to the table is thread safe. Modification of the table is
// not.

#pragma once

#include <vector>

#include "astro_core/earth/internal/table.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class EarthOrientationTable {
 public:
  // Add earth orientation information row to the table.
  //
  // The mjd is the time point in the modified Julian date (MJD) format measured
  // in the UTC time scale. The Earth orientation parameters are measured or
  // predicted at that time.
  //
  // The rows can be added in any order, but no duplicate data detection is
  // performed.
  //
  // After table is modified Preprocess() is to be called.
  void AddRow(const ModifiedJulianDate& mjd,
              const Vec2& polar_motion,
              const double ut1_minus_utc) {
    table_.push_back({mjd, polar_motion, ut1_minus_utc});
  }

  // Pre-process the data in the table rows, making the table fast for lookups.
  // It is to be called by table importers once after the data has been loaded.
  void Preprocess();

  // Lookup UT1-UTC in seconds for the given time in UTC scale provided in the
  // modified Julian date (MJD) format.
  //
  // Clamping extrapolation strategy is used. This means that for the dates
  // outside of what the table provides a value from the closest row is
  // returned. For example, for a date prior to the first table row the value
  // from the first row is returned. Likewise, for a date past the last row of
  // the table the value from the last row is returned.
  auto LookupUT1MinusUTCSecondsInUTCScale(const ModifiedJulianDate& mjd) const
      -> double;

  // Lookup polar motion in arcsec for the given time in UTC scale provided in
  // the modified Julian date (MJD) format.
  //
  // Clamping extrapolation strategy is used. This means that for the dates
  // outside of what the table provides a value from the closest row is
  // returned. For example, for a date prior to the first table row the value
  // from the first row is returned. Likewise, for a date past the last row of
  // the table the value from the last row is returned.
  auto LookupPolarMotionArcsecInUTCScale(const ModifiedJulianDate& mjd) const
      -> Vec2;

 private:
  // Specification of a row of the table.
  struct Row {
    // Modified Julian date (MJD) in UTC scale.
    ModifiedJulianDate mjd;

    // A polar motion values, arcsec.
    Vec2 polar_motion;

    // UT1-UTC, is seconds.
    // A value from Bulletin B is used when available, otherwise Bulletin A is
    // used.
    double ut1_minus_utc{0};
  };

  // Content of the table.
  earth_internal::Table<Row> table_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
