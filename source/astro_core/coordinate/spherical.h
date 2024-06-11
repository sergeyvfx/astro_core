// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// 3D point position and velocity representation in spherical coordinate system.

#pragma once

#include <iosfwd>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Cartesian;
class CartesianDifferential;

class Spherical {
 public:
  // Construct default spherical coordinate with all fields set to 0.
  Spherical() = default;

  // Initialization from the given parameters.
  //
  // The parameters follow the semantic of the fields of the coordinate.
  //
  // This constructor allows to construct coordinate using designated
  // initializer to improve readability of the meaning of the passed values.
  // The non-specified fields are initialized to their default values.
  //
  // Example:
  //
  //   Spherical coord({.latitude = lat, .longitude = lon, .distance = r});
  struct Parameters {
    double latitude{0};
    double longitude{0};
    double distance{0};
  };
  explicit constexpr Spherical(const Parameters& parameters)
      : latitude(parameters.latitude),
        longitude(parameters.longitude),
        distance(parameters.distance) {}

  explicit Spherical(const Cartesian& cartesian);

  // Convert representation to cartesian:
  //   - X axis is aligned with latitude and longitude of 0.
  //   - Y axis is aligned with latitude of 0 and longitude of pi/2.
  //   - Z axis is aligned with latitude of pi/2 and longitude of 0.
  auto ToCartesian() const -> Cartesian;

  // Convert representation to spherical.
  // Is a no-op, intended for use in generic templated code.
  constexpr inline auto ToSpherical() const -> const Spherical& {
    return *this;
  }

  // Check for an exact equality.
  inline bool operator==(const Spherical& other) const = default;
  inline bool operator!=(const Spherical& other) { return !(*this == other); }

  friend auto operator<<(std::ostream& os, const Spherical& r) -> std::ostream&;

  // Geodetic latitude, north/south, radians.
  // Canonically is [-pi/2 .. pi/2], 0 is the equator.
  double latitude = 0;

  // Longitude, east/west, radians.
  // Canonically is [-pi .. pi].
  double longitude = 0;

  // Distance to the point.
  double distance = 0;
};

class SphericalDifferential {
 public:
  // Construct default spherical differentials with all fields set to 0.
  SphericalDifferential() = default;

  // Initialization from the given parameters.
  //
  // The parameters follow the semantic of the fields of the coordinate.
  //
  // This constructor allows to construct coordinate using designated
  // initializer to improve readability of the meaning of the passed values.
  // The non-specified fields are initialized to their default values.
  //
  // Example:
  //
  //   SphericalDifferential coord({.d_latitude = lat,
  //                                .d_longitude = lon,
  //                                .d_distance = r});
  struct Parameters {
    double d_latitude{0};
    double d_longitude{0};
    double d_distance{0};
  };
  explicit constexpr SphericalDifferential(const Parameters& parameters)
      : d_latitude(parameters.d_latitude),
        d_longitude(parameters.d_longitude),
        d_distance(parameters.d_distance) {}

  // Convert representation to spherical.
  // Is a no-op, intended for use in generic templated code.
  //
  // TODO(sergey): Add position as an argument to make it consistent with the
  // ToCartesian().
  constexpr inline auto ToSpherical() const -> const SphericalDifferential& {
    return *this;
  }
  constexpr inline auto ToSpherical(const Spherical& /*position*/) const
      -> const SphericalDifferential& {
    return *this;
  }
  constexpr inline auto ToSpherical(const Cartesian& /*position*/) const
      -> const SphericalDifferential& {
    return *this;
  }

  // Convert representation to spherical.
  //
  // The position is position of observed object at the moment when the velocity
  // was measured.
  //
  // This is an equivalent of the velocity transform from ERFA eraPv2s().
  auto ToCartesian(const Spherical& position) const -> CartesianDifferential;
  auto ToCartesian(const Cartesian& position) const -> CartesianDifferential;

  // Check for an exact equality.
  inline bool operator==(const SphericalDifferential& other) const = default;
  inline bool operator!=(const SphericalDifferential& other) {
    return !(*this == other);
  }

  friend auto operator<<(std::ostream& os,
                         const SphericalDifferential& r) -> std::ostream&;

  // Differential of latitude. Radians per unit time.
  double d_latitude = 0;

  // Differential of longitude. Radians per unit time.
  double d_longitude = 0;

  // Differential of distance, unit distance per unit time.
  double d_distance = 0;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
