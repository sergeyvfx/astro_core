// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Type definition, and base mathematical operations of a mathematical vector.
//
// Provides Eigen-style vector implementation with the major differences being:
//
//  - The naming follows CamelCase.
//  - Initialization of constant vector can be done by passing values to the
//    constructor (no stream API to put values to the vector is implemented).
//  - No SIMD vectorization.
//  - Printing is done in a row and not in a column.
//
// When or if this module needs to have much more sophisticated linear algebra
// algorithms then it is probably better to switch to Eigen.

#pragma once

#include <array>
#include <cstddef>
#include <ostream>
#include <span>
#include <type_traits>
#include <utility>

#include "astro_core/base/algorithm.h"
#include "astro_core/base/unroll.h"
#include "astro_core/math/math.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace numeric_internal {

namespace detail {

// Implementation of vector cross-product for different templated sizes.
//
// It is only specialized for vector sizes 2 and 3 and for other sizes generates
// a static assert message that the operands of requested sizes are not
// supported.
template <size_t kLHSSize, size_t kLHS>
struct Cross;

}  // namespace detail

// Declaration and implementation of the vector itself.
template <class T, size_t N>
class Vector {
 public:
  //////////////////////////////////////////////////////////////////////////////
  // Member types.

  // Type of elements in this vector.
  using ScalarType = T;

  //////////////////////////////////////////////////////////////////////////////
  // Constants.

  // Dimension of the vector provided at compile time.
  static constexpr size_t kStaticDimension = N;

  //////////////////////////////////////////////////////////////////////////////
  // Constructors.

  // Create vector created with all elements set to T{0}.
  static auto Zero() -> Vector { return Constant(T{0}); }

  // Create vector created with all elements set to value.
  static auto Constant(const T& value) -> Vector {
    Vector result;
    Unroll<N>([&](const auto i) { result.data_[i] = value; });
    return result;
  }

  // Construct initialized vector.
  // There is no explicit value provided to the elements, which could result in
  // an uninitialized state if T is not a class.
  Vector() = default;

  // Construct vector with the given values.
  //
  // The number of values should be less of equal to the vector size. The
  // elements of the vector for which the value is not provided are
  // value-initialized.
  template <class... Types,
            std::enable_if_t<(N == 1) && sizeof...(Types) == N, int> = 0>
  explicit constexpr inline Vector(Types... s) : data_{static_cast<T>(s)...} {}
  template <class... Types,
            std::enable_if_t<(N > 1) && sizeof...(Types) == N, int> = 0>
  constexpr inline Vector(Types... s) : data_{static_cast<T>(s)...} {}

  //////////////////////////////////////////////////////////////////////////////
  // Shape.

  // Dimension of the vector - the number of its elements.
  consteval inline auto GetDimension() const -> size_t { return N; }

  //////////////////////////////////////////////////////////////////////////////
  // Element access.

  // A view to a continuous memory containing values of elements of this vector.
  constexpr inline auto Data() -> std::span<T> { return data_; }
  constexpr inline auto Data() const -> std::span<const T> { return data_; }

  // An unsized pointer to a continuous memory containing values of elements of
  // this vector.
  constexpr inline auto Pointer() -> T* { return Data().data(); }
  constexpr inline auto Pointer() const -> const T* { return Data().data(); }

  // Return a reference to the element at specified location pos.
  // No bounds checking is performed.
  constexpr inline auto operator()(const size_t pos) -> T& {
    return data_[pos];
  }
  constexpr inline auto operator()(const size_t pos) const -> const T& {
    return data_[pos];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparison.

  // Compare for an exact per-element match between two vector.
  friend constexpr inline auto operator==(const Vector& lhs, const Vector& rhs)
      -> bool {
    for (size_t i = 0; i < N; ++i) {
      if (lhs(i) != rhs(i)) {
        return false;
      }
    }
    return true;
  }

  friend constexpr inline auto operator!=(const Vector& lhs,
                                          const Vector& rhs) {
    return !(lhs == rhs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operations.

  // Return the per-element negation of this vector.
  constexpr inline auto operator-() const -> Vector {
    Vector result;
    Unroll<N>([&](const auto i) { result.data_[i] = -data_[i]; });
    return result;
  }

  // Add a vector to this one.
  constexpr inline auto operator+=(const Vector& rhs) -> Vector& {
    Unroll<N>([&](const auto i) { data_[i] += rhs.data_[i]; });
    return *this;
  }

  // Subtract a vector from this one.
  constexpr inline auto operator-=(const Vector& rhs) -> Vector& {
    Unroll<N>([&](const auto i) { data_[i] -= rhs.data_[i]; });
    return *this;
  }

  // Multiply the vector by a scalar.
  template <class OtherScalarType>
  constexpr inline auto operator*=(const OtherScalarType& rhs) -> Vector& {
    Unroll<N>([&](const auto i) { data_[i] = T(data_[i] * rhs); });
    return *this;
  }

  // Divide the vector by a scalar.
  template <class OtherScalarType>
  constexpr inline auto operator/=(const OtherScalarType& rhs) -> Vector& {
    Unroll<N>([&](const auto i) { data_[i] = T(data_[i] / rhs); });
    return *this;
  }

  // Add two vectors.
  // This is a per-element sum.
  friend inline constexpr auto operator+(const Vector& lhs, const Vector& rhs)
      -> Vector {
    Vector result{lhs};
    result += rhs;
    return result;
  }

  // Subtract rhs vector from lhs vector.
  // This is a per-element difference.
  friend inline constexpr auto operator-(const Vector& lhs, const Vector& rhs)
      -> Vector {
    Vector result{lhs};
    result -= rhs;
    return result;
  }

  // Multiply vector by a scalar.
  // Multiplies every element of the vector by the scalar.
  template <class OtherScalarType>
  friend inline constexpr auto operator*(const Vector& lhs,
                                         const OtherScalarType& rhs) -> Vector {
    Vector result{lhs};
    result *= rhs;
    return result;
  }

  // Multiply scalar by vector.
  // Multiplies every element of the vector by the scalar.
  template <class OtherScalarType>
  friend inline constexpr auto operator*(const OtherScalarType& lhs,
                                         const Vector& rhs) -> Vector {
    Vector result{rhs};
    result *= lhs;
    return result;
  }

  // Divide vector by a scalar.
  // Divides every element of the vector by the scalar.
  template <class OtherScalarType>
  friend inline constexpr auto operator/(const Vector& lhs,
                                         const OtherScalarType& rhs) -> Vector {
    Vector result{lhs};
    result /= rhs;
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Algebraic functions.

  // Squared L2 norm: sum of the square of all the vector elements.
  constexpr inline auto SquaredNorm() const -> T {
    T squared_norm{};
    Unroll<N>([&](const auto i) { squared_norm += data_[i] * data_[i]; });
    return squared_norm;
  }

  // Squared L2 norm: square root of the sum of the square of all the vector
  // elements.
  constexpr inline auto Norm() const -> T { return Sqrt(SquaredNorm()); }

  // Calculate cross-product between this vector and given right hand side
  // vector.
  // Only implemented for vector sizes of 2 and 3.
  // If one of the vectors has size 3 then the result is vector od size 3.
  // If Both vectors have size 2 then the output is a scalar.
  template <size_t kRHSSize>
  constexpr inline auto Cross(const Vector<T, kRHSSize>& rhs) const {
    return detail::Cross<N, kRHSSize>::Calculate(*this, rhs);
  }

  // Calculate dot-product between this vector and given right hand side vector.
  constexpr inline auto Dot(const Vector& rhs) const -> T {
    T dot_product{};
    Unroll<N>([&](const auto i) { dot_product += data_[i] * rhs.data_[i]; });
    return dot_product;
  }

  // Return vector which has the same direction as this one, but has a unit
  // length.
  constexpr inline auto Normalized() const -> Vector { return *this / Norm(); }

  // Coefficient-wise absolute value.
  constexpr inline auto CwiseAbs() const -> Vector {
    Vector abs;
    Unroll<N>([&](const auto i) { abs(i) = Abs(data_[i]); });
    return abs;
  }

  // Coefficient-wise minimum between this vector and the other:
  //   RESULT[i] = Min(self(i), other[i]) for i = 0 to N
  constexpr inline auto CwiseMin(const Vector& other) const -> Vector {
    Vector min;
    Unroll<N>([&](const auto i) { min(i) = Min(data_[i], other(i)); });
    return min;
  }

  // Coefficient-wise maximum between this vector and the other:
  //   RESULT[i] = Max(self(i), other[i]) for i = 0 to N
  constexpr inline auto CwiseMax(const Vector& other) const -> Vector {
    Vector min;
    Unroll<N>([&](const auto i) { min(i) = Max(data_[i], other(i)); });
    return min;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output.

  friend auto operator<<(std::ostream& os, const Vector& vec) -> std::ostream& {
    Unroll<N>([&](const auto i) {
      if constexpr (i) {
        os << " ";
      }
      os << vec(i);
    });
    return os;
  }

 private:
  std::array<T, N> data_;
};

namespace detail {

// Helper to deduct the type of scalar needed to hold the cross-product
// operation result.
template <class T, class U>
struct CrossResult {
  using Type = decltype(std::declval<T>() * std::declval<U>() -
                        std::declval<T>() * std::declval<U>());
};

template <class T, class U>
using CrossResultType = typename CrossResult<T, U>::Type;

// Vec3 × Vec3.
template <>
struct Cross<3, 3> {
  template <class T, class U>
  static constexpr inline auto Calculate(const Vector<T, 3>& lhs,
                                         const Vector<U, 3>& rhs) {
    return Vector<CrossResultType<T, U>, 3>(lhs(1) * rhs(2) - lhs(2) * rhs(1),
                                            lhs(2) * rhs(0) - lhs(0) * rhs(2),
                                            lhs(0) * rhs(1) - lhs(1) * rhs(0));
  }
};

// Vec3 × Vec2.
template <>
struct Cross<3, 2> {
  template <class T, class U>
  static constexpr inline auto Calculate(const Vector<T, 3>& lhs,
                                         const Vector<U, 2>& rhs) {
    return Vector<CrossResultType<T, U>, 3>(
        -lhs(2) * rhs(1), lhs(2) * rhs(0), lhs(0) * rhs(1) - lhs(1) * rhs(0));
  }
};

// Vec2 × Vec3.
template <>
struct Cross<2, 3> {
  template <class T, class U>
  static constexpr inline auto Calculate(const Vector<T, 2>& lhs,
                                         const Vector<U, 3>& rhs) {
    return Vector<CrossResultType<T, U>, 3>(
        lhs(1) * rhs(2), -lhs(0) * rhs(2), lhs(0) * rhs(1) - lhs(1) * rhs(0));
  }
};

// Vec2 × Vec2.
template <>
struct Cross<2, 2> {
  template <class T>
  static constexpr inline auto Calculate(const Vector<T, 2>& lhs,
                                         const Vector<T, 2>& rhs) {
    return lhs(0) * rhs(1) - lhs(1) * rhs(0);
  }
};

}  // namespace detail

}  // namespace numeric_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core

////////////////////////////////////////////////////////////////////////////////
// Integration with the Google testing framework.
//
// The specialization of the StlContainerView for the Vector type allows to use
// Pointwise operations from the testing framework.
//
// Example:
//   EXPECT_THAT(CalculateVec2(), Pointwise(DoubleNear(1e-12), {1.23, 4.56}));
namespace testing::internal {
template <class ContainerType>
class StlContainerView;

template <class T, size_t N>
class StlContainerView<astro_core::numeric_internal::Vector<T, N>> {
  using Vector = astro_core::numeric_internal::Vector<T, N>;

 public:
  using type = std::array<T, N>;
  using const_reference = std::span<const T>;

  static auto ConstReference(const Vector& vec) -> const_reference {
    return vec.Data();
  }

  static auto Copy(const Vector& vec) -> type {
    const const_reference data = vec.Data();
    type copy;
    std::copy(data.begin(), data.end(), copy.begin());
    return copy;
  }
};

}  // namespace testing::internal
