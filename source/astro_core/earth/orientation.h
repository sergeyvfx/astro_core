// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Earth orientation parameters.

#pragma once

#include "astro_core/numeric/numeric.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class EarthOrientationData;
class ModifiedJulianDate;

// Get global data which holds the tabulated information about Earth orientation
// parameters. This data is used by functions in this file. So this is required
// to provide the data before functions will return meaningful results.
auto GetEarthOrientationData() -> EarthOrientationData&;

// Lookup UT1-UTC in seconds for the given time in UTC scale provided in the
// modified Julian date (MJD) format.
//
// Uses the global data to obtain Earth orientation parameters It is required
// tables are provided to the data obtained via GetEarthOrientationData(). If
// the data is not provided (0, 0) is returned.
//
// If the table has not been provided then 0 is returned.
//
// This is an equivalent of Astropy's Time.get_delta_ut1_utc().
auto GetUT1MinusUTCSecondsInUTCScale(const ModifiedJulianDate& mjd_utc)
    -> double;

// Calculate polar motion in radians for the given time in UTC scale provided in
// the modified Julian date (MJD) format.
//
// Uses the global data to obtain Earth orientation parameters It is required
// tables are provided to the data obtained via GetEarthOrientationData(). If
// the data is not provided (0, 0) is returned.
//
// This is an equivalent of Astropy's IERS_A.pm_xy() and IERS_B.pm_xy().
auto GetEarthPolarMotionInUTCScale(const ModifiedJulianDate& mjd_utc) -> Vec2;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
