// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second.h"

#include "astro_core/earth/leap_second_data.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto GetLeapSecondData() -> LeapSecondData& {
  static LeapSecondData leap_second_data;
  return leap_second_data;
}

auto GetTAIMinusUTCSecondsInUTCScale(const ModifiedJulianDate& mjd_utc)
    -> double {
  const LeapSecondData& leap_second_data = GetLeapSecondData();
  return leap_second_data.LookupTAIMinusUTCSecondsInUTCScale(mjd_utc);
}

auto GetTAIMinusUTCSecondsInTAIScale(const ModifiedJulianDate& mjd_tai)
    -> double {
  const LeapSecondData& leap_second_data = GetLeapSecondData();
  return leap_second_data.LookupTAIMinusUTCSecondsInTAIScale(mjd_tai);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
