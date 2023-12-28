// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// True Equator Mean Equinox (TEME) frame.
//
// This frame is used by SGP4 model and the intent of it is to provide an
// efficient, if approximate, coordinate system for use with the AFSPC
// analytical theories. An exact operational definition of TEME is very
// difficult to find in the literature.

#pragma once

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/frame.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ITRF;

class TEME : public PositionVelocityFrame<Cartesian, CartesianDifferential> {
  using BaseClass = PositionVelocityFrame<Cartesian, CartesianDifferential>;

 public:
  using BaseClass::BaseClass;

  static auto FromITRF(const ITRF& itrf) -> TEME;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
