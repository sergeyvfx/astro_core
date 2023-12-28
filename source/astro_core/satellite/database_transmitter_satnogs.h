// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parser of satellite transmitter information provided in JSON format by the
// SatNOGS [1] project.
//
// [1] Open Source global network of satellite ground-stations
//     https://satnogs.org/
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

// Parse the json which contains transmitter information
//
// The database is to be populated with the satellite information prior to using
// this parser.
auto LoadSatNOGSTransmitters(SatelliteDatabase& database, std::string_view text)
    -> bool;

}  // namespace experimental
}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
