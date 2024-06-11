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

auto SphericalDifferential::ToCartesian(const Spherical& position) const
    -> CartesianDifferential {
  // The formulas in readable form can be found at
  //
  //   http://www.astrosurf.com/jephem/library/li110spherCart_en.htm
  //
  // NOTE: It seems the signs in the (formulae 2) are inverted.
  //
  // TODO(sergey): Find a source which derives exact formulas matching ERFA for
  // the ease of understanding.

  const double sin_theta = Sin(position.longitude);
  const double cos_theta = Cos(position.longitude);
  const double sin_phi = Sin(position.latitude);
  const double cos_phi = Cos(position.latitude);

  Vec3 cartesian;
  cartesian(0) = cos_phi * cos_theta * d_distance -
                 position.distance * cos_phi * sin_theta * d_longitude -
                 position.distance * sin_phi * cos_theta * d_latitude;
  cartesian(1) = cos_phi * sin_theta * d_distance +
                 position.distance * cos_phi * cos_theta * d_longitude -
                 position.distance * sin_phi * sin_theta * d_latitude;
  cartesian(2) =
      sin_phi * d_distance + position.distance * cos_phi * d_latitude;

  return {cartesian};
}

auto SphericalDifferential::ToCartesian(const Cartesian& position) const
    -> CartesianDifferential {
  return ToCartesian(position.ToSpherical());
}

auto operator<<(std::ostream& os,
                const SphericalDifferential& v) -> std::ostream& {
  os << "(d_latitude: " << v.d_latitude << ", d_longitude: " << v.d_longitude
     << ", d_distance: " << v.d_distance << ")";
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
