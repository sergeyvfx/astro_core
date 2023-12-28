// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Loader of 3 line element data into a database of satellites.
//
// The 3 line element consists of a line with the satellite name, followed with
// the 2 lines with the satellite TLE.
//
// This is the format of database provided by CelesTrak.
//
// NOTE: This is an experimental API, it might get changed or even moved outside
// of the library.

#pragma once

#include <string_view>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

class SatelliteDatabase;

// Parse the text which contains 3LE information about satellites into the
// database.
//
// Returns true if all records from the text have been successfully parsed and
// added to the database.
//
// If the database is not empty, the records will either be added or updated for
// the new TLE from the text.
//
// There is no check for duplicate satellite catalog numbers in the 3LE text.
auto Load3LE(SatelliteDatabase& database, std::string_view text) -> bool;

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
