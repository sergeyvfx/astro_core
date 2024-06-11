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
      .longitude = ArcTan2(y, x),
      .distance = distance,
  });
}

auto operator<<(std::ostream& os, const Cartesian& r) -> std::ostream& {
  os << "(x: " << r.x << ", y: " << r.y << ", z: " << r.z << ")";
  return os;
}

auto CartesianDifferential::ToSpherical(const Cartesian& position) const
    -> SphericalDifferential {
  // The formulas in readable form can be found at
  //
  //   http://www.astrosurf.com/jephem/library/li110spherCart_en.htm
  //
  // NOTE: It seems the sign of formulas for longitude and latitude is inverted
  // compared to the Astropy and ERFA. Possibly a mistake when solving the
  // equation.
  //
  // TODO(sergey): Find a source which derives exact formulas matching ERFA for
  // the ease of understanding.

  Vec3 r = position;

  double x2y2 = r(0) * r(0) + r(1) * r(1);  // x^2 + y^2
  double x2y2z2 = x2y2 + r(2) * r(2);       // x^2 + y^2 + z^2

  if (x2y2z2 == 0) {
    // Trick from ERFA: for zero position move the position in the direction of
    // velocity. This avoids allows to have non-zero position without changing
    // the direction of velocity.
    r = Vec3(d_x, d_y, d_z);
    x2y2 = r(0) * r(0) + r(1) * r(1);  // x^2 + y^2
    x2y2z2 = x2y2 + r(2) * r(2);       // x^2 + y^2 + z^2
  }

  SphericalDifferential spherical;

  if (x2y2 != 0) {
    spherical.d_latitude =
        (x2y2 * d_z - r(2) * (r(0) * d_x + r(1) * d_y)) / (x2y2z2 * Sqrt(x2y2));
    spherical.d_longitude = (r(0) * d_y - d_x * r(1)) / x2y2;
  } else {
    spherical.d_latitude = 0.0;
    spherical.d_longitude = 0.0;
  }

  if (x2y2z2 != 0) {
    spherical.d_distance =
        (r(0) * d_x + r(1) * d_y + r(2) * d_z) / Sqrt(x2y2z2);
  } else {
    spherical.d_distance = 0.0;
  }

  return spherical;
}

auto CartesianDifferential::ToSpherical(const Spherical& position) const
    -> SphericalDifferential {
  return ToSpherical(position.ToCartesian());
}

auto operator<<(std::ostream& os,
                const CartesianDifferential& v) -> std::ostream& {
  os << "(dx: " << v.d_x << ", dy: " << v.d_y << ", dz: " << v.d_z << ")";
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
