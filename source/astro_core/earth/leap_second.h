// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Leap second information

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ModifiedJulianDate;
class LeapSecondData;

// Get global data which holds the tabulated information about leap second.
// This data is used by functions in this file. So this is required
// to provide the data before functions will return meaningful results.
auto GetLeapSecondData() -> LeapSecondData&;

// Calculate TAI-UTC value for the given time in MJD format and UTC scale.
//
// If the date is prior to the known period of the TAI-UTC 0 is returned.
// If the table has not been provided then for time range where table is
// needed zero is returned.
auto GetTAIMinusUTCSecondsInUTCScale(const ModifiedJulianDate& mjd_utc)
    -> double;

// Calculate TAI-UTC value for the given time in MJD format and TAI scale.
//
// If the date is prior to the known period of the TAI-UTC 0 is returned.
// If the table has not been provided then for time range where table is
// needed zero is returned.
auto GetTAIMinusUTCSecondsInTAIScale(const ModifiedJulianDate& mjd_tai)
    -> double;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
