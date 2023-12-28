// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Leap seconds table which specifies TAI-UTC differences.
//
// The table defines the run-time format which is needed for its operation and
// methods to obtain the number of leap seconds for the given time point. It
// does not define the format from which the data for it is obtained.
//
// The lookup access to the table is thread safe. Modification of the table is
// not.

#pragma once

#include <vector>

#include "astro_core/earth/internal/table.h"
#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class LeapSecondTable {
 public:
  // Add leap second row to the table.
  //
  // The mjd is the time point in the modified Julian date (MJD) format measured
  // in the UTC time scale. In indicates time from which the new TAI-UTC value
  // is effective.
  //
  // The rows can be added in any order, but no duplicate data detection is
  // performed.
  //
  // After table is modified Preprocess() is to be called.
  void AddRow(const ModifiedJulianDate& mjd, const double tai_minus_utc) {
    table_.push_back({mjd, {}, tai_minus_utc});
  }

  // Pre-process the data in the table rows, making the table fast for lookups.
  // It is to be called by table importers once after the data has been loaded.
  void Preprocess();

  // Lookup TAI-UTC value for the given time in MJD format and UTC scale.
  //
  // Clamping extrapolation strategy is used. This means that for the dates
  // outside of what the table provides a value from the closest row is
  // returned. For example, for a date prior to the first table row the value
  // from the first row is returned. Likewise, for a date past the last row of
  // the table the value from the last row is returned.
  //
  // Technically, with the up-to-date table it is possible to look up dates up
  // to almost 1 year into the future. However, no trust level checks are
  // performed on the returned value.
  //
  // If the table is empty then 0 is returned.
  auto LookupTAIMinusUTCSecondsInUTCScale(
      const ModifiedJulianDate& mjd_utc) const -> double;

  // Lookup TAI-UTC value for the given time in MJD format and TAU scale.
  //
  // Clamping extrapolation strategy is used. This means that for the dates
  // outside of what the table provides a value from the closest row is
  // returned. For example, for a date prior to the first table row the value
  // from the first row is returned. Likewise, for a date past the last row of
  // the table the value from the last row is returned.
  //
  // Technically, with the up-to-date table it is possible to look up dates up
  // to almost 1 year into the future. However, no trust level checks are
  // performed on the returned value.
  //
  // If the table is empty then 0 is returned.
  auto LookupTAIMinusUTCSecondsInTAIScale(
      const ModifiedJulianDate& mjd_tai) const -> double;

 private:
  // Specification of a row of the table.
  struct Row {
    // Modified Julian date (MJD) starting from which the TAI-UTC value defined
    // in this row is valid from. The TAI-UTC is valid until the MJD time point
    // defined in the next row of the table.
    //
    // The date in both UTC and TAI time scales are stored to allow fast lookup
    // of TAI-UTC in either of the scales.
    ModifiedJulianDate mjd_utc;
    ModifiedJulianDate mjd_tai;

    // TAI-UTC in seconds.
    double tai_minus_utc;
  };

  // Content of the table.
  earth_internal::Table<Row> table_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
