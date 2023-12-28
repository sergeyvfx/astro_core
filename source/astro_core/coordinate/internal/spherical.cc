// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/spherical.h"

#include <iostream>

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/math/math.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

Spherical::Spherical(const Cartesian& cartesian)
    : Spherical(cartesian.ToSpherical()) {}

auto Spherical::ToCartesian() const -> Cartesian {
  double sin_lat, cos_lat;
  SinCos(latitude, sin_lat, cos_lat);

  double sin_lon, cos_lon;
  SinCos(longitude, sin_lon, cos_lon);

  return {
      distance * cos_lat * cos_lon,
      distance * cos_lat * sin_lon,
      distance * sin_lat,
  };
}

auto operator<<(std::ostream& os, const Spherical& r) -> std::ostream& {
  os << "(latitude: " << r.latitude << ", longitude: " << r.longitude
     << ", distance: " << r.distance << ")";
  return os;
}

auto operator<<(std::ostream& os, const SphericalDifferential& v)
    -> std::ostream& {
  os << "(d_latitude: " << v.d_latitude << ", d_longitude: " << v.d_longitude
     << ", d_distance: " << v.d_distance << ")";
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
