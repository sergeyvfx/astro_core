// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Matrix and vector classes.

#pragma once

#include "astro_core/math/math.h"
#include "astro_core/numeric/internal/matrix.h"
#include "astro_core/numeric/internal/vector.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

////////////////////////////////////////////////////////////////////////////////
// Data types.

// Base vector class. Consists of N elements of type T.
template <class T, size_t N>
using Vec = numeric_internal::Vector<T, N>;

// Base matrix class.
template <class T, size_t kNumRows, size_t kNumColumns>
using Mat = numeric_internal::Matrix<T, kNumRows, kNumColumns>;

// Fixed-size vectors of commonly used types and sizes.
using Vec2 = Vec<double, 2>;
using Vec3 = Vec<double, 3>;
using Vec4 = Vec<double, 4>;

// Fixed-size matrices of commonly used types and sizes.
using Mat2 = Mat<double, 2, 2>;
using Mat3 = Mat<double, 3, 3>;
using Mat4 = Mat<double, 4, 4>;

////////////////////////////////////////////////////////////////////////////////
// Functions.

// Construct matrix which rotates vector by a given angle (in radians) about the
// X axis counterclockwise when looking towards the origin.
inline auto VectorRotationAroundX(const double angle) -> Mat3 {
  double sin, cos;
  SinCos(angle, sin, cos);

  return Mat3::FromColumns({
      {1.0, 0.0, 0.0},
      {0.0, cos, sin},
      {0.0, -sin, cos},
  });
}

// Construct matrix which rotates vector by a given angle (in radians) about the
// Y axis counterclockwise when looking towards the origin.
inline auto VectorRotationAroundY(const double angle) -> Mat3 {
  double sin, cos;
  SinCos(angle, sin, cos);

  return Mat3::FromColumns({
      {cos, 0.0, -sin},
      {0.0, 1.0, 0.0},
      {sin, 0.0, cos},
  });
}

// Construct matrix which rotates vector by a given angle (in radians) about the
// X axis counterclockwise when looking towards the origin.
inline auto VectorRotationAroundZ(const double angle) -> Mat3 {
  double sin, cos;
  SinCos(angle, sin, cos);

  return Mat3::FromColumns({
      {cos, sin, 0.0},
      {-sin, cos, 0.0},
      {0.0, 0.0, 1.0},
  });
}

// Construct matrix which rotates axes by a given angle (in radians) about the
// corresponding axis counterclockwise when looking towards the origin.
inline auto AxisRotationAroundX(const double angle) -> Mat3 {
  return VectorRotationAroundX(-angle);
}
inline auto AxisRotationAroundY(const double angle) -> Mat3 {
  return VectorRotationAroundY(-angle);
}
inline auto AxisRotationAroundZ(const double angle) -> Mat3 {
  return VectorRotationAroundZ(-angle);
}

////////////////////////////////////////////////////////////////////////////////
// Aliases of axis rotation matrices to match the naming used in the books like
// [Vallado2013] and various papers paper.
//
// References:
//
//   [Vallado2013] Vallado, David A., and James Wertz.
//     Fundamentals of Astrodynamics and Applications. 2013.

inline auto ROT1(const double angle) -> Mat3 {
  return AxisRotationAroundX(angle);
}
inline auto ROT2(const double angle) -> Mat3 {
  return AxisRotationAroundY(angle);
}
inline auto ROT3(const double angle) -> Mat3 {
  return AxisRotationAroundZ(angle);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
