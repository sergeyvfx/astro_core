// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Type definition, and base mathematical operations of a matrix.
// The storage order is column major.
//
// Provides Eigen-style matrix implementation with the major differences being:
//
//  - The naming follows CamelCase.
//  - Initialization of constant matrix can be done by passing values to the
//    constructor (no stream API to put values to the matrix is implemented).
//  - No SIMD vectorization.
//
// When or if this module needs to have much more sophisticated linear algebra
// algorithms then it is probably better to switch to Eigen.

#pragma once

#include <array>
#include <cstddef>
#include <ostream>
#include <type_traits>
#include <utility>

#include "astro_core/base/algorithm.h"
#include "astro_core/base/unroll.h"
#include "astro_core/numeric/internal/vector.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace numeric_internal {

template <class T, size_t kNumRows, size_t kNumColumns>
class Matrix {
  using ColumnType = std::array<T, kNumRows>;
  using MatrixType = std::array<ColumnType, kNumColumns>;

  static_assert(sizeof(ColumnType) == sizeof(T) * kNumRows);
  static_assert(sizeof(MatrixType) == sizeof(T) * kNumRows * kNumColumns);

 public:
  //////////////////////////////////////////////////////////////////////////////
  // Member types.

  // Type of elements in this matrix.
  using ScalarType = T;

  //////////////////////////////////////////////////////////////////////////////
  // Constants.

  // Dimension of the matrix provided at compile time.
  static constexpr size_t kStaticNumRows = kNumRows;
  static constexpr size_t kStaticNumColumns = kNumColumns;

  //////////////////////////////////////////////////////////////////////////////
  // Constructors.

  // Create matrix created with all elements set to T{0}.
  static auto Zero() -> Matrix {
    Matrix result;
    Unroll<kNumColumns, kNumRows>(
        [&](const auto column, const auto row) { result(row, column) = T{0}; });
    return result;
  }

  // Create an identity matrix with its diagonal set to 1 and the rest of
  // elements set to 0.
  //
  // Example:
  //
  //   std::cout << Matrix<int, 2, 2>::Identity() << std::endl;
  //
  // Output:
  //
  //   1 0 0
  //   0 1 0
  static auto Identity() -> Matrix {
    Matrix result = Zero();
    Unroll<Min(kNumRows, kNumColumns)>(
        [&](const auto i) { result.data_[i][i] = T{1}; });
    return result;
  }

  // Construct matrix from provided columns.
  // The data is in column-major order, meaning to initialize the matrix
  //
  //   ( a b c )
  //   ( d e f )
  //
  // is initialized with data = { {a,b}, {c,d}, {e,f} }
  template <class DataT,
            int kNumDataRows,
            int kNumDataColumns,
            class = std::enable_if_t<std::is_convertible_v<DataT, T>>>
  static auto FromColumns(const DataT (&data)[kNumDataColumns][kNumDataRows])
      -> Matrix {
    static_assert(kNumDataColumns == kNumColumns);
    static_assert(kNumDataRows == kNumRows);

    Matrix result;
    Unroll<kNumColumns, kNumRows>([&](const auto column, const int row) {
      result(row, column) = data[column][row];
    });
    return result;
  }

  // Construct matrix from provided rows.
  // The data is in row-major order, meaning to initialize the matrix
  //
  //   ( a b c )
  //   ( d e f )
  //
  // is initialized with data = { {a,b,c}, {d,e,f} }
  template <class DataT,
            int kNumDataRows,
            int kNumDataColumns,
            class = std::enable_if_t<std::is_convertible_v<DataT, T>>>
  static auto FromRows(const DataT (&data)[kNumDataRows][kNumDataColumns])
      -> Matrix {
    static_assert(kNumDataColumns == kNumColumns);
    static_assert(kNumDataRows == kNumRows);

    Matrix result;
    Unroll<kNumColumns, kNumRows>([&](const auto column, const int row) {
      result(row, column) = data[row][column];
    });
    return result;
  }

  // Construct default initialized matrix.
  // There is no explicit value provided to the elements, which could result in
  // an uninitialized state if T is not a class.
  Matrix() = default;

  //////////////////////////////////////////////////////////////////////////////
  // Shape.

  // Dimension of the matrix.
  consteval inline auto GetNumRows() const -> size_t { return kNumRows; }
  consteval inline auto GetNumColumns() const -> size_t { return kNumColumns; }

  //////////////////////////////////////////////////////////////////////////////
  // Element access.

  // A view to a continuous memory containing values of elements of this matrix.
  // The elements in the array are in the colexicographical access order.
  constexpr inline auto Data() -> std::span<T> {
    return std::span(&data_[0][0], kNumRows * kNumColumns);
  }
  constexpr inline auto Data() const -> std::span<const T> {
    return std::span(&data_[0][0], kNumRows * kNumColumns);
  }

  // An unsized pointer to a continuous memory containing values of elements of
  // this matrix.
  constexpr inline auto Pointer() -> T* { return Data().data(); }
  constexpr inline auto Pointer() const -> const T* { return Data().data(); }

  // Return a reference to the element at specified row and column.
  // No bounds checking is performed.
  constexpr inline auto operator()(const size_t row, const size_t column)
      -> T& {
    return data_[column][row];
  }
  constexpr inline auto operator()(const size_t row, const size_t column) const
      -> const T& {
    return data_[column][row];
  }

  //////////////////////////////////////////////////////////////////////////////
  // Comparison.

  // Compare for an exact per-element match between two matrices.
  friend constexpr inline auto operator==(const Matrix& lhs, const Matrix& rhs)
      -> bool {
    for (size_t column = 0; column < kNumColumns; ++column) {
      for (size_t row = 0; row < kNumRows; ++row) {
        if (lhs(row, column) != rhs(row, column)) {
          return false;
        }
      }
    }
    return true;
  }

  friend constexpr inline auto operator!=(const Matrix& lhs,
                                          const Matrix& rhs) {
    return !(lhs == rhs);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operations.

  // Return the per-element negation of this matrix.
  constexpr inline auto operator-() const -> Matrix {
    Matrix result;
    Unroll<kNumColumns, kNumRows>([&](const auto column, const int row) {
      result(row, column) = (*this)(row, column);
    });
    return result;
  }

  // Add a matrix to this one.
  // This is a per-element sum.
  constexpr inline auto operator+=(const Matrix& rhs) -> Matrix& {
    Unroll<kNumColumns, kNumRows>([&](const auto column, const int row) {
      (*this)(row, column) += rhs(row, column);
    });
    return *this;
  }

  // Subtract a matrix from this one.
  // This is a per-element difference.
  constexpr inline auto operator-=(const Matrix& rhs) -> Matrix& {
    Unroll<kNumColumns, kNumRows>([&](const auto column, const int row) {
      (*this)(row, column) -= rhs(row, column);
    });
    return *this;
  }

  // Multiply the matrix by a scalar.
  // Multiplies every element of the matrix by the scala.
  constexpr inline auto operator*=(const T& rhs) -> Matrix& {
    Unroll<kNumColumns, kNumRows>(
        [&](const auto column, const int row) { (*this)(row, column) *= rhs; });
    return *this;
  }

  // Divide the matrix by a scalar.
  constexpr inline auto operator/=(const T& rhs) -> Matrix& {
    Unroll<kNumColumns, kNumRows>(
        [&](const auto column, const int row) { (*this)(row, column) /= rhs; });
    return *this;
  }

  // Add two matrices.
  // This is a per-element sum.
  friend inline constexpr auto operator+(const Matrix& lhs, const Matrix& rhs)
      -> Matrix {
    Matrix result{lhs};
    result += rhs;
    return result;
  }

  // Subtract rhs matrix from lhs matrix.
  // This is a per-element difference.
  friend inline constexpr auto operator-(const Matrix& lhs, const Matrix& rhs)
      -> Matrix {
    Matrix result{lhs};
    result -= rhs;
    return result;
  }

  // Multiply matrix by a scalar.
  // Multiplies every element of the matrix by the scalar.
  friend inline constexpr auto operator*(const Matrix& lhs, const T& rhs)
      -> Matrix {
    Matrix result{lhs};
    result *= rhs;
    return result;
  }

  // Multiply scalar by matrix.
  // Multiplies every element of the matrix by the scalar.
  friend inline constexpr auto operator*(const T& lhs, const Matrix& rhs)
      -> Matrix {
    Matrix result{rhs};
    result *= lhs;
    return result;
  }

  // Multiply the matrix by a matrix.
  template <size_t kOtherNumColumns>
  friend inline constexpr auto operator*(
      const Matrix& lhs, const Matrix<T, kNumColumns, kOtherNumColumns>& rhs) {
    Matrix<T, kNumRows, kOtherNumColumns> result;
    Unroll<kOtherNumColumns, kNumRows>([&](const auto column, const int row) {
      T sum{0};
      Unroll<kNumColumns>(
          [&](const auto i) { sum += lhs(row, i) * rhs(i, column); });
      result(row, column) = sum;
    });
    return result;
  }

  // Divide matrix by a scalar.
  // Divides every element of the matrix by the scalar.
  friend inline constexpr auto operator/(const Matrix& lhs, const T& rhs)
      -> Matrix {
    Matrix result{lhs};
    result /= rhs;
    return result;
  }

  // Matrix-vector multiplication.
  friend inline constexpr auto operator*(const Matrix& m,
                                         const Vector<T, kNumColumns>& v)
      -> Vector<T, kNumRows> {
    Vector<T, kNumRows> result;
    Unroll<kNumRows>([&](const auto i) {
      result(i) = T{0};
      Unroll<kNumColumns>([&](const auto j) { result(i) += m(i, j) * v(j); });
    });
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Arithmetic.

  // Return matrix which is mirrored around the diagonal by swapping rows and
  // columns:
  //
  //   (1 2 3)^T = (1 4)
  //   (4 5 6)     (2 5)
  //               (3 6)
  constexpr inline auto Transposed() const -> Matrix<T, kNumColumns, kNumRows> {
    Matrix<T, kNumColumns, kNumRows> result;
    Unroll<kNumColumns, kNumRows>([&](const auto column, const auto row) {
      result(column, row) = (*this)(row, column);
    });
    return result;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Output.

  friend auto operator<<(std::ostream& os, const Matrix& m) -> std::ostream& {
    Unroll<kNumRows>([&](const auto row) {
      if constexpr (row != 0) {
        os << "\n";
      }
      Unroll<kNumColumns>([&](const auto column) {
        if constexpr (column != 0) {
          os << " ";
        }
        os << m(row, column);
      });
    });
    return os;
  }

 private:
  MatrixType data_{};
};

}  // namespace numeric_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core

////////////////////////////////////////////////////////////////////////////////
// Integration with the Google testing framework.
//
// The specialization of the StlContainerView for the Matrix type allows to use
// Pointwise operations from the testing framework.
//
// Example:
//   EXPECT_THAT(CalculateMatrix(), Pointwise(DoubleNear(1e-12), {1.23, 4.56}));
namespace testing::internal {

template <class ContainerType>
class StlContainerView;

template <class T, size_t kNumRows, size_t kNumColumns>
class StlContainerView<
    astro_core::numeric_internal::Matrix<T, kNumRows, kNumColumns>> {
  using Matrix = astro_core::numeric_internal::Matrix<T, kNumRows, kNumColumns>;

 public:
  using type = std::array<T, kNumRows * kNumColumns>;
  using const_reference = std::span<const T>;

  static auto ConstReference(const Matrix& m) -> const_reference {
    return m.Data();
  }

  static auto Copy(const Matrix& m) -> type {
    const const_reference data = m.Data();
    type copy;
    std::copy(data.begin(), data.end(), copy.begin());
    return copy;
  }
};

}  // namespace testing::internal
