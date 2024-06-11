// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// 3D point position and velocity representation in cartesian coordinate system.

#pragma once

#include <iosfwd>

#include "astro_core/numeric/numeric.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Spherical;
class CartesianDifferential;
class SphericalDifferential;

// Point in 3D cartesian coordinate system.
class Cartesian {
 public:
  // Construct default cartesian coordinate at the origin (0, 0, 0).
  constexpr Cartesian() = default;

  constexpr Cartesian(const double new_x,
                      const double new_y,
                      const double new_z)
      : x(new_x), y(new_y), z(new_z) {}

  // Conversion form and to Vec3.
  constexpr Cartesian(const Vec3& vector)
      : x(vector(0)), y(vector(1)), z(vector(2)) {}
  inline operator Vec3() const { return {x, y, z}; }

  // Disable indirect implicit conversion of differential to position (via
  // CartesianDifferential -> Vec3 -> Cartesian).
  Cartesian(const CartesianDifferential& other) = delete;

  explicit Cartesian(const Spherical& other);

  // Convert representation to cartesian.
  // Is a no-op, intended for use in generic templated code.
  constexpr inline auto ToCartesian() const -> const Cartesian& {
    return *this;
  }

  // Convert representation to spherical:
  //   - X axis is aligned with latitude and longitude of 0.
  //   - Y axis is aligned with latitude of 0 and longitude of pi/2.
  //   - Z axis is aligned with latitude of pi/2 and longitude of 0.
  auto ToSpherical() const -> Spherical;

  // Check for an exact equality.
  inline bool operator==(const Cartesian& other) const = default;
  inline bool operator!=(const Cartesian& other) const {
    return !(*this == other);
  }

  // Check for an exact equality with a vector.
  // Semantically the same as cartesian == Cartesian(vec3).
  inline bool operator==(const Vec3& other) const {
    return x == other(0) && y == other(1) && z == other(2);
  }
  inline bool operator!=(const Vec3& other) const { return !(*this == other); }

  friend auto operator<<(std::ostream& os, const Cartesian& r) -> std::ostream&;

  double x = 0;
  double y = 0;
  double z = 0;
};

// Differential (velocity) of a point in 3D cartesian coordinate system.
class CartesianDifferential {
 public:
  constexpr CartesianDifferential() = default;
  constexpr CartesianDifferential(const double new_d_x,
                                  const double new_d_y,
                                  const double new_d_z)
      : d_x(new_d_x), d_y(new_d_y), d_z(new_d_z) {}

  // Conversion form and to Vec3.
  constexpr CartesianDifferential(const Vec3& vector)
      : d_x(vector(0)), d_y(vector(1)), d_z(vector(2)) {}
  inline operator Vec3() const { return {d_x, d_y, d_z}; }

  // Disable indirect implicit conversion of position to differential(via
  // Cartesian -> Vec3 -> CartesianDifferential).
  CartesianDifferential(const Cartesian& other) = delete;

  // Convert representation to cartesian.
  // Is a no-op, intended for use in generic templated code.
  //
  // TODO(sergey): Add position as an argument to make it consistent with the
  // ToSpherical().
  constexpr inline auto ToCartesian() const -> const CartesianDifferential& {
    return *this;
  }
  constexpr inline auto ToCartesian(const Cartesian& /*position*/) const
      -> const CartesianDifferential& {
    return *this;
  }
  constexpr inline auto ToCartesian(const Spherical& /*position*/) const
      -> const CartesianDifferential& {
    return *this;
  }

  // Convert representation to spherical.
  //
  // The position is position of observed object at the moment when the velocity
  // was measured.
  //
  // This is an equivalent of the velocity transform from ERFA eraPv2s().
  auto ToSpherical(const Cartesian& position) const -> SphericalDifferential;
  auto ToSpherical(const Spherical& position) const -> SphericalDifferential;

  // Check for an exact equality.
  inline bool operator==(const CartesianDifferential& other) const = default;
  inline bool operator!=(const CartesianDifferential& other) {
    return !(*this == other);
  }

  // Check for an exact equality with a vector.
  // Semantically the same as cartesian == CartesianDifferential(vec3).
  inline bool operator==(const Vec3& other) const {
    return d_x == other(0) && d_y == other(1) && d_z == other(2);
  }
  inline bool operator!=(const Vec3& other) const { return !(*this == other); }

  friend auto operator<<(std::ostream& os,
                         const CartesianDifferential& v) -> std::ostream&;

  double d_x = 0;
  double d_y = 0;
  double d_z = 0;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
