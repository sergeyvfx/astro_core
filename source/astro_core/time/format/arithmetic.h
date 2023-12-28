// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Base class and boiler plate for formats which stores time as an
// arithmetic-like property and wants to support basic arithmetical operations.
//
// The storage type ValueType and the final tome format class FormatType are
// provided as a template arguments. Specifying the concrete format class allows
// to limit operations to that class only, avoiding accidental mix of different
// time formats in a single formula.
//
// The ValueType needs to implement the basic arithmetical operations such as
// unary and binary addition, subtraction, multiplication, and division.
//
// The following methods and functions are implemented:
//
//   - Default constructor, which ues default initialization for the value.
//   - Explicit conversion to double and the ValueType.
//   - Negation operator
//   - Unary operators +=, -=, *=, /=.
//   - Binary operators +, -, *, /.
//
// Example:
//
//   class MyFormat : public ArithmeticFormat<DoubleDouble, MyFormat> {
//     using BaseClass = ArithmeticFormat<DoubleDouble, MyFormat>;
//
//    public:
//     using BaseClass::BaseClass;
//
//     using BaseClass::operator<=>;
//     using BaseClass::operator==;
//   };
//
//   ...
///
//   MyFormat fmt{10};
//   fmt += 10;

#pragma once

#include <iostream>
#include <type_traits>
#include <utility>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

template <class ValueType, class FormatType>
class ArithmeticFormat {
  // Check for the type T is acceptable as an arithmetic operand.
  template <class T>
  using IsCompatibleArithmeticType =
      std::disjunction<std::is_arithmetic<T>,
                       std::is_same<T, ValueType>,
                       std::is_same<T, FormatType>>;

  // SFINAE helper to enable function only if the type T is acceptable as an
  // arithmetic operand.
  template <class T>
  using EnableIfCompatibleArithmeticType =
      std::enable_if_t<IsCompatibleArithmeticType<T>::value>;

  // SFINAE helper to enable function only if the types T and U can be used as
  // left hand side and right hand side respectively as operands of arithmetic
  // operation.
  template <class T, class U>
  using EnableIfCompatibleBinaryArithmeticTypes = std::enable_if_t<
      std::conjunction<IsCompatibleArithmeticType<T>,
                       IsCompatibleArithmeticType<U>,
                       std::disjunction<std::is_same<T, FormatType>,
                                        std::is_same<U, FormatType>>>::value>;

  // Get value suitable for arithmetic operation from the given value.
  template <class T>
  static constexpr inline auto GetValue(const T& value) -> const T& {
    return value;
  }
  static constexpr inline auto GetValue(const FormatType& value)
      -> const ValueType& {
    return value.value_;
  }

 public:
  // Default constructor.
  // The value is default-initialized.
  ArithmeticFormat() = default;

  constexpr ArithmeticFormat(const ValueType& value) : value_(value) {}

  ArithmeticFormat(const ArithmeticFormat& other) = default;
  ArithmeticFormat(ArithmeticFormat&& other) noexcept = default;

  ~ArithmeticFormat() = default;

  //////////////////////////////////////////////////////////////////////////////
  // Assignment.

  auto operator=(const ArithmeticFormat& other) -> ArithmeticFormat& = default;
  auto operator=(ArithmeticFormat&& other) -> ArithmeticFormat& = default;

  // Assign the value.
  auto operator=(const ValueType& value) -> ArithmeticFormat& {
    value_ = value;
    return *this;
  }
  auto operator=(ValueType&& value) -> ArithmeticFormat& {
    if (this != &value) {
      value_ = std::move(value);
    }
    return *this;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Access.

  // Get scalar representation of the date.
  //
  // NOTE: Conversion to double floating point could be lossy.
  constexpr explicit operator const ValueType&() const { return value_; }
  constexpr explicit operator double() const { return double(value_); }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operations.

  // Get the value which value is the negative of this value.
  constexpr auto operator-() const -> ArithmeticFormat {
    return ArithmeticFormat(-value_);
  }

  // Unary addition.
  template <class T, class = EnableIfCompatibleArithmeticType<T>>
  constexpr auto operator+=(const T& rhs) -> ArithmeticFormat& {
    value_ += GetValue(rhs);
    return *this;
  }

  // Unary subtraction.
  template <class T, class = EnableIfCompatibleArithmeticType<T>>
  constexpr auto operator-=(const T& rhs) -> ArithmeticFormat& {
    value_ -= GetValue(rhs);
    return *this;
  }

  // Unary multiplication.
  template <class T, class = EnableIfCompatibleArithmeticType<T>>
  constexpr auto operator*=(const T& rhs) -> ArithmeticFormat& {
    value_ *= GetValue(rhs);
    return *this;
  }

  // Unary division.
  template <class T, class = EnableIfCompatibleArithmeticType<T>>
  constexpr auto operator/=(const T& rhs) -> ArithmeticFormat& {
    value_ /= GetValue(rhs);
    return *this;
  }

  // Comparison with other object.
  constexpr auto operator<=>(const FormatType& other) const {
    return value_ <=> other.value_;
  }
  constexpr auto operator==(const FormatType& other) const -> bool {
    return value_ == other.value_;
  }

  // Comparison with non-typed modified Julian date value.
  // Equivalent to comparison with an implicitly created ArithmeticFormat from
  // the given mjd value.
  constexpr auto operator<=>(const ValueType& value) const {
    return value_ <=> value;
  }
  constexpr auto operator==(const ValueType& value) const -> bool {
    return value_ == value;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Non-member mathematical functions.

  // Binary addition.
  template <class T,
            class U,
            class = EnableIfCompatibleBinaryArithmeticTypes<T, U>>
  friend constexpr auto operator+(const T& lhs, const U& rhs) -> FormatType {
    FormatType result{lhs};
    result.value_ += GetValue(rhs);
    return result;
  }

  // Binary subtraction.
  template <class T,
            class U,
            class = EnableIfCompatibleBinaryArithmeticTypes<T, U>>
  friend constexpr auto operator-(const T& lhs, const U& rhs) -> FormatType {
    FormatType result{lhs};
    result.value_ -= GetValue(rhs);
    return result;
  }

  // Binary multiplication.
  template <class T,
            class U,
            class = EnableIfCompatibleBinaryArithmeticTypes<T, U>>
  friend constexpr auto operator*(const T& lhs, const U& rhs) -> FormatType {
    FormatType result{lhs};
    result.value_ *= GetValue(rhs);
    return result;
  }

  // Binary division.
  template <class T,
            class U,
            class = EnableIfCompatibleBinaryArithmeticTypes<T, U>>
  friend constexpr auto operator/(const T& lhs, const U& rhs) -> FormatType {
    FormatType result{lhs};
    result.value_ /= GetValue(rhs);
    return result;
  }

 protected:
  ValueType value_{};
};

// Computes the absolute value of a value arg.
template <class ValueType, class Format>
constexpr auto Abs(const ArithmeticFormat<ValueType, Format>& arg)
    -> ArithmeticFormat<ValueType, Format> {
  return arg < 0 ? -arg : arg;
}

template <class ValueType, class Format>
inline auto operator<<(std::ostream& os,
                       const ArithmeticFormat<ValueType, Format>& arg)
    -> std::ostream& {
  os << ValueType(arg);
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
