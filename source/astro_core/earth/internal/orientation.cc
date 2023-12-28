// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Earth orientation parameters.

#include "astro_core/earth/orientation.h"

#include "astro_core/earth/orientation_data.h"
#include "astro_core/math/math.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto GetEarthOrientationData() -> EarthOrientationData& {
  static EarthOrientationData earth_orientation_data;
  return earth_orientation_data;
}

auto GetUT1MinusUTCSecondsInUTCScale(const ModifiedJulianDate& mjd_utc)
    -> double {
  const EarthOrientationData& earth_orientation_data =
      GetEarthOrientationData();

  return earth_orientation_data.LookupUT1MinusUTCSecondsInUTCScale(mjd_utc);
}

auto GetEarthPolarMotionInUTCScale(const ModifiedJulianDate& mjd_utc) -> Vec2 {
  const EarthOrientationData& earth_orientation_data =
      GetEarthOrientationData();

  return ArcsecToRadians(
      earth_orientation_data.LookupPolarMotionArcsecInUTCScale(mjd_utc));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
