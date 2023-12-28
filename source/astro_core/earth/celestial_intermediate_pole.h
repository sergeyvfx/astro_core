// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/numeric/numeric.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Calculate X, Y coordinates of celestial intermediate pole from series based
// on IAU 2006 precession and IAU 2000A nutation.
//
// This is an equivalent of ERFA's eraXy06().
auto CelestialIntermediatePole(const JulianDate& jd_tt) -> Vec2;

// Calculate the CIO locator s, positioning the Celestial Intermediate Origin on
// the equator of the Celestial Intermediate Pole, given the CIP's X,Y
// coordinates.
//
// Compatible with IAU 2006/2000A precession-nutation.
//
// The time is given in Julian Date format and Terrestrial Time scale.
// The CIP coordinate is calculated by CelestialIntermediatePole() for the same
// time point.
//
// Returns the CIO locator s in radians.
//
// This is an equivalent of ERFA's eraS06().
auto CelestialIntermediateOriginLocator(const JulianDate& jd_tt,
                                        const Vec2& cip_xy) -> double;

// Form the celestial to intermediate-frame-of-date matrix given the CIP X,Y and
// the CIO locator s.
//
// This is an equivalent of ERFA's eraC2ixys().
auto CelestialToIntermediateFrameOfDateMatrix(const Vec2& cip_xy,
                                              const double s) -> Mat3;

// Form the celestial to intermediate-frame-of-date matrix given the CIP X,Y and
// the CIO locator s at the given time point in Terrestrial TIme scale in Julian
// date format.
//
// Produces the same result as
//
//   const Vec2 xy = CelestialIntermediatePole(jd_tt);
//   const double s = CelestialIntermediateOriginLocator(jd_tt, xy);
//   const Mat3 m = CelestialToIntermediateFrameOfDateMatrix(xy, s);
//
// but with less internal calculations.
auto CelestialToIntermediateFrameOfDateMatrix(const JulianDate& jd_tt) -> Mat3;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
