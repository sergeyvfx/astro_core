// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation_data.h"

#include <array>

#include "astro_core/earth/orientation_table.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Hide the implementation, so that it is possible to only have forward
// declaration of Table in the header.
EarthOrientationData::EarthOrientationData() = default;
EarthOrientationData::~EarthOrientationData() = default;

void EarthOrientationData::SetTable(Table&& table) {
  shared_table_.Set(std::move(table));
}

auto EarthOrientationData::LookupUT1MinusUTCSecondsInUTCScale(
    const ModifiedJulianDate& mjd) const -> double {
  const SharedTable::LocalTable local_table = shared_table_.Load();
  if (!local_table) {
    return 0;
  }

  return local_table->LookupUT1MinusUTCSecondsInUTCScale(mjd);
}

auto EarthOrientationData::LookupPolarMotionArcsecInUTCScale(
    const ModifiedJulianDate& mjd) const -> Vec2 {
  const SharedTable::LocalTable local_table = shared_table_.Load();
  if (!local_table) {
    return {0, 0};
  }

  return local_table->LookupPolarMotionArcsecInUTCScale(mjd);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
