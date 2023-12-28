// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/cartesian.h"

#include <iostream>

#include "astro_core/coordinate/spherical.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/numeric.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

Cartesian::Cartesian(const Spherical& other) : Cartesian(other.ToCartesian()) {}

auto Cartesian::ToSpherical() const -> Spherical {
  const double distance = Vec3(*this).Norm();

  if (distance == 0.0) {
    return Spherical();
  }

  return Spherical({
      .latitude = ArcSin(z / distance),
      .longitude = atan2(y, x),
      .distance = distance,
  });
}

auto operator<<(std::ostream& os, const Cartesian& r) -> std::ostream& {
  os << "(x: " << r.x << ", y: " << r.y << ", z: " << r.z << ")";
  return os;
}

auto operator<<(std::ostream& os, const CartesianDifferential& v)
    -> std::ostream& {
  os << "(dx: " << v.d_x << ", dy: " << v.d_y << ", dz: " << v.d_z << ")";
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
