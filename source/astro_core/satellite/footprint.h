// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Footprint of a satellite on the surface of Earth.
//
// This footprint designates an area on which the satellite is above an
// observer's horizon.
//
// A simplified model of a spherical Earth without taking mountains or valleys
// into account.

#pragma once

#include <span>

#include "astro_core/coordinate/geographic.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Geodetic;

// Calculate points of a satellite footprint.
//
// The satellite is denoted by its position in geodetic coordinates.
// The footprint is denotes by points of a poly-line which denote the boundary
// of the satellite visibility. The winding of these points is not specified.
//
// The number of footprint points is expected to be even. If it is odd, then
// an extra point at the end is added which matches the starting point.
//
// Due to the way how the algorithm works even when the number of points is
// even, there will be 2 duplicates: this is because of the symmetrical nature
// of the footprint relative to the projection of the satellite.
void CalculateSatelliteFootprint(const Geodetic& satellite_position,
                                 std::span<Geographic> footprint);

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
