// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Earth parameters.
//
// References:
//
//   [Vallado2006] Vallado, David A., Paul Crawford, Richard Hujsak, and T.S.
//       Kelso, "Revisiting Spacetrack Report #3," presented at the AIAA/AAS
//       Astrodynamics Specialist Conference, Keystone, CO, 2006 August 21–24.
//
//  [WGS] World Geodetic System
//        https://en.wikipedia.org/wiki/World_Geodetic_System

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace earth_internal {

enum class System {
  WGS72,
  WGS80,
  WGS84,
};

struct Ellipsoid {
  double a;  //  Semi-major axis.
  double f;  //  flattening.
};

template <System kSystem>
struct GetEllipsoid;

template <>
struct GetEllipsoid<System::WGS72> {
  consteval static auto Get() -> Ellipsoid { return {6378135.0, 1.0 / 298.26}; }
};

template <>
struct GetEllipsoid<System::WGS80> {
  consteval static auto Get() -> Ellipsoid {
    return {6378137.0, 1.0 / 298.257222100882711};
  }
};

template <>
struct GetEllipsoid<System::WGS84> {
  consteval static auto Get() -> Ellipsoid {
    return {6378137.0, 1.0 / 298.257223563};
  }
};

}  // namespace earth_internal

struct Earth {
  using System = earth_internal::System;

  // Rational velocity.
  //
  // [Vallado2006] Eq. (3-75).
  // Matches astropy.coordinates.earth.OMEGA_EARTH
  inline static constexpr double kOmega{7.292115146706979e-5};

  // Parameters of the Earth ellipsoid [WGS].
  using Ellipsoid = earth_internal::Ellipsoid;

  template <System kSystem>
  using GetEllipsoid = earth_internal::GetEllipsoid<kSystem>;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
