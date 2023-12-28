// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// The Geocentric Celestial Reference Frame (GCRF)
//
// Source:
// https://en.wikipedia.org/wiki/Equatorial_coordinate_system
// https://search.r-project.org/CRAN/refmans/asteRisk/html/GCRFtoITRF.html
//
//   The GCRF (Geocentric Celestial Reference Frame) frame of reference is an
//   Earth-centered inertial coordinate frame, where the origin is placed at the
//   center of mass of Earth and the coordinate frame is fixed with respect to
//   the stars (and therefore not fixed with respect to the Earth surface in its
//   rotation). The X-axis is aligned with the mean equinox of Earth at 12:00
//   Terrestrial Time on the 1st of January, 2000, and the Z-axis is aligned
//   with the Earth´s rotation axis.

#pragma once

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/frame.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ITRF;

class GCRF : public PositionVelocityFrame<Cartesian, CartesianDifferential> {
  using BaseClass = PositionVelocityFrame<Cartesian, CartesianDifferential>;

 public:
  using BaseClass::BaseClass;

  static auto FromITRF(const ITRF& teme) -> GCRF;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
