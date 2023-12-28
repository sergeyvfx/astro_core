// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Helper function to provide all known parameter tables, such as Earth
// orientation parameters, leap seconds, and so on.

#pragma once

#include "astro_core/earth/internal/leap_second_test_data.h"
#include "astro_core/earth/internal/orientation_test_data.h"
#include "astro_core/earth/leap_second.h"
#include "astro_core/earth/leap_second_data.h"
#include "astro_core/earth/leap_second_table.h"
#include "astro_core/earth/orientation.h"
#include "astro_core/earth/orientation_data.h"
#include "astro_core/earth/orientation_table.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace test_data {

inline void SetTables() {
  LeapSecondTable leap_second_table = CreateLeapSecondTable();
  GetLeapSecondData().SetTable(std::move(leap_second_table));

  EarthOrientationTable earth_orientation_table = CreateEarthOrientationTable();
  GetEarthOrientationData().SetTable(std::move(earth_orientation_table));
}

}  // namespace test_data
}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
