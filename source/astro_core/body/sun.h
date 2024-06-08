// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//  [almanac08] U.S. Naval Observatory; U.K. Hydrographic Office, H.M. Nautical
//    Almanac Office (2008). The Astronomical Almanac for the Year 2010. U.S.
//    Govt. Printing Office. p. C5. ISBN 978-0-7077-4082-9.
//
//  [usno] U.S. Naval Observatory, Approximate Solar Coordinates.
//    https://aa.usno.navy.mil/faq/sun_approx
//    https://web.archive.org/web/20181115153648/http://aa.usno.navy.mil/faq/docs/SunApprox.php

#pragma once

#include "astro_core/coordinate/gcrf.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Approximate position of the Sun at the given date.
//
// Implements an algorithm described in the [almanac08] with modifications from
// [usno] to expand the accurate range to years 1800 to 2200.
//
// The algorithm gives result within 60 arcsec for right ascension and
// declination. The distance is within 11km.
auto GetApproximateSunCoordinate(const Time& time) -> GCRF;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
