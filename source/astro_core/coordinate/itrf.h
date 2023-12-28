// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// International Terrestrial Reference Frame (ITRF)
//
// Source:
// https://en.wikipedia.org/wiki/International_Terrestrial_Reference_System_and_Frame
//
//   The International Terrestrial Reference System (ITRS) describes procedures
//   for creating reference frames suitable for use with measurements on or near
//   the Earth's surface. The ITRS defines a geocentric system of coordinates
//   using the SI system of measurement.
//
//   An International Terrestrial Reference Frame (ITRF) is a realization of the
//   ITRS. Its origin is at the center of mass of the whole earth including the
//   oceans and atmosphere.
//
// This frame is also often referred as ECEF.

#pragma once

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/frame.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class GCRF;
class TEME;
class Geodetic;

class ITRF : public PositionVelocityFrame<Cartesian, CartesianDifferential> {
  using BaseClass = PositionVelocityFrame<Cartesian, CartesianDifferential>;

 public:
  using BaseClass::BaseClass;

  static auto FromGCRF(const GCRF& gcrf) -> ITRF;
  static auto FromTEME(const TEME& teme) -> ITRF;
  static auto FromGeodetic(const Geodetic& geodetic) -> ITRF;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
