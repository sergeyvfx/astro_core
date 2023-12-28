// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Utilities to help calculating doppler shift of radio frequencies transmitted
// from a satellite.

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ITRF;

// Calculate the factor of the doppler effect:
//
//   f = doppler_factor * f0
//
// where f0 is the frequency transmitted from the satellite and f is the
// frequency received by an observer.
auto CalculateDopplerFactor(const ITRF& site, const ITRF& satellite) -> double;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
