// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/earth/earth.h"
#include "astro_core/math/math.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// The geocentric radius is the distance from the Earth's center to a point on
// the spheroid surface at geodetic latitude.
//
// The latitude is given in radians.
inline auto GeocentricRadius(const Earth::Ellipsoid& ellipsoid,
                             const double geodetic_latitude) -> double {
  // Implements formula from
  //
  //  https://en.wikipedia.org/wiki/Earth_radius#Geocentric_radius

  const double a = ellipsoid.a;  // equatorial radius a
  const double f = ellipsoid.f;  // flattening of the ellipsoid f

  const double b = a - a * f;  // polar radius b

  double sin_phi, cos_phi;
  SinCos(geodetic_latitude, sin_phi, cos_phi);

  const double a_cos_phi = a * cos_phi;
  const double b_sin_phi = b * sin_phi;

  const double a2_cos_phi = a * a_cos_phi;
  const double b2_sin_phi = b * b_sin_phi;

  // Expand the expression and simplify the argument passed to the Sqrt.
  // If the expression is written as a naive formula (without temporary
  // variables) then the test fails when is build with Apple clang version
  // 14.0.3 (clang-1403.0.22.14.1) on Intel CPU.
  const double a2_cos_phi2 = a2_cos_phi * a2_cos_phi;
  const double b2_sin_phi2 = b2_sin_phi * b2_sin_phi;
  const double a_cos_phi2 = a_cos_phi * a_cos_phi;
  const double b_sin_phi2 = b_sin_phi * b_sin_phi;

  return Sqrt((a2_cos_phi2 + b2_sin_phi2) / (a_cos_phi2 + b_sin_phi2));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
