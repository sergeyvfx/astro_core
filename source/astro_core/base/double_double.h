// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// High-precision floating-point representation.
//
// The simplified idea is to stor the floating-point value as a part of double
// precision floating-point values, with one of them storing a "coarse" value
// and the second one storing the "correction" to it.
//
// The implementation is based on algorithms from various sources.
//
// The biggest unknown is whether algorithms choosen here and in other libraries
// (i.e. [FPplus]) maintain the uniqueness of representation of the
// double-double number. The uniqueness is required for comparison.
//
// Some work (i.e. [QD2000],  [Priest1997]) uses re-normalization after
// mathematical operations. The [QD2000] algorithm seems to not behave correctly
// for an input of (-3072.6144, 0, 0). The [Priest1997] algorithm works fine but
// is computationally expensive, and also unknown if it is guaranteed to give
// the exact required number of doubles. Simplified algorithms are used here,
// following other libraries.
//
// TODO(sergey): Figure out whether the algorithms maintain uniqueness of the
// double-double representation.
//
// References:
//
//   [Dekker1971] Theodorus Jozef Dekker. 1971. A floating-point technique for
//       extending the available precision. Numerische Mathematik 18, 3 (1971),
//       224–242.
//
//   [FPHandbook2009] Jean-Michel Muller, Nicolas Brisebarre, Florent De
//       Dinechin, Claude-Pierre Jeannerod, Vincent Lefevre, Guillaume
//       Melquiond, Nathalie Revol, Damien Stehlé , and Serge Torres. 2009.
//       Handbook of floating-point arithmetic. Springer Science & Business
//       Media.
//
//   [FPplus] Scientific library for high-precision computations and research
//            https://github.com/Maratyszcza/FPplus
//
//   [Priest1997] Douglas M. Priest. On Properties of Floating Point
//       Arithmetics: Numerical Stability and the Cost of Accurate Computations.
//       PhD thesis, University of California, Berkeley, November 1992.
//
//   [Shewchuk1997] Jonathan Richard Shewchuk. 1997. Adaptive precision
//       floating-point arithmetic and fast robust geometric predicates.
//       Discrete & Computational Geometry 18, 3 (1997), 305–363.
//
//       https://people.eecs.berkeley.edu/~jrs/papers/robustr.pdf
//
//   [QD2000] Yozo Hida, Xiaoye S Li, and David H Bailey. 2000. Quad-double
//       arithmetic: Algorithms, implementation, and application. In 15th IEEE
//       Symposium on Computer Arithmetic, Citeseer, 155–162.
//
//       https://www.davidhbailey.com/dhbpapers/quad-double.pdf

#pragma once

#include <cstdint>
#include <ostream>

#include "astro_core/base/compiler_specific.h"
#include "astro_core/math/math.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace double_double_internal {

// The value representation.
// This is an unevaluated sum of double floating-point values.
//
// There is no single representation of a floating-point value as a sum of
// two values. The implementation of the double-double type maintains the
// values as a non-overlapping expansion.
struct Value {
  // The part of the sum with the largest magnitude.
  double x;

  // The part of the sum with the smallest magnitude.
  double y;
};

}  // namespace double_double_internal

// Floating-point-like number which implements basic high-precision
// mathematical operations like addition and multiplication.
class DoubleDouble {
 public:
  //////////////////////////////////////////////////////////////////////////////
  // Constructors and default assignment.

  DoubleDouble() = default;

  // Construct double-double value from the given floating-point scalar value.
  constexpr DoubleDouble(const double value) : DoubleDouble(value, 0) {}

  // Construct double-double value so that its value matches (in an unevaluated
  // sense) `a + b`.
  constexpr DoubleDouble(const double a, const double b);

  DoubleDouble(const DoubleDouble& other) = default;

  auto operator=(const DoubleDouble& other) -> DoubleDouble& = default;

  //////////////////////////////////////////////////////////////////////////////
  // Access.

  // Get largest and lowest by magnitude part of the number.
  constexpr auto GetHi() const -> double { return value_.x; };
  constexpr auto GetLo() const -> double { return value_.y; };

  // Access the value as a double-precision floating-point scalar.
  //
  // NOTE: This is a lossy operation in a sense that the return value looses
  // precision compared to what the double-double allows to maintain.
  explicit constexpr operator double() const { return value_.x + value_.y; }

  //////////////////////////////////////////////////////////////////////////////
  // Mathematical operations.

  // Get the value which value is the negative of this value.
  constexpr auto operator-() const -> DoubleDouble;

  // Addition.
  constexpr auto operator+=(double rhs) -> DoubleDouble&;
  constexpr auto operator+=(const DoubleDouble& rhs) -> DoubleDouble;

  // Subtraction.
  constexpr auto operator-=(double rhs) -> DoubleDouble&;
  constexpr auto operator-=(const DoubleDouble& rhs) -> DoubleDouble;

  // Multiplication.
  constexpr auto operator*=(double rhs) -> DoubleDouble;
  constexpr auto operator*=(const DoubleDouble& rhs) -> DoubleDouble;

  // Division.
  constexpr auto operator/=(double rhs) -> DoubleDouble;
  constexpr auto operator/=(const DoubleDouble& rhs) -> DoubleDouble;

  // Comparison.
  friend constexpr auto operator<=>(const DoubleDouble& lhs,
                                    const DoubleDouble& rhs);
  friend constexpr auto operator==(const DoubleDouble& lhs,
                                   const DoubleDouble& rhs) -> bool;

 private:
  double_double_internal::Value value_;
};

////////////////////////////////////////////////////////////////////////////////
// Non-member functions.

// Addition.
constexpr auto operator+(const DoubleDouble& lhs, double rhs) -> DoubleDouble;
constexpr auto operator+(double lhs, const DoubleDouble& rhs) -> DoubleDouble;
constexpr auto operator+(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble;

// Subtraction.
constexpr auto operator-(const DoubleDouble& lhs, double rhs) -> DoubleDouble;
constexpr auto operator-(double lhs, const DoubleDouble& rhs) -> DoubleDouble;
constexpr auto operator-(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble;

// Multiplication.
constexpr auto operator*(const DoubleDouble& lhs, double rhs) -> DoubleDouble;
constexpr auto operator*(double lhs, const DoubleDouble& rhs) -> DoubleDouble;
constexpr auto operator*(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble;

// Division.
constexpr auto operator/(const DoubleDouble& lhs, double rhs) -> DoubleDouble;
constexpr auto operator/(double lhs, const DoubleDouble& rhs) -> DoubleDouble;
constexpr auto operator/(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble;

// Computes the absolute value of a value arg.
inline auto Abs(const DoubleDouble& arg) -> DoubleDouble;

// Truncate the value to the integer part.
inline auto Trunc(const DoubleDouble& x) -> DoubleDouble;

// TODO(sergey): Implement other mathematical functions like Sqrt, Sin, Cos.
// See [QD2000] Section 4.

inline auto operator<<(std::ostream& os, const DoubleDouble& value)
    -> std::ostream&;

////////////////////////////////////////////////////////////////////////////////
// Implementation of low-level double-double operations.

namespace double_double_internal {

// Round to nearest even tie-breaking for the sum of a and b.
constexpr auto AddRoundToNearestEven(const double a, const double b) -> double {
  // Round to nearest even is the default default mode in the IEEE 754-2008
  // Standard [FPHandbook2009].
  return a + b;
}

// Round to nearest even tie-breaking for the product of a and b.
constexpr auto MultiplyRoundToNearestEven(const double a, const double b)
    -> double {
  // Round to nearest even is the default default mode in the IEEE 754-2008
  // Standard [FPHandbook2009].
  return a * b;
}

// Calculate non-overlapping a non-overlapping expansion `x + y` such that
// `a + b = x + y`, where `x` is an approximation to `a + b` and `y` is the
// roundoff error in the calculation of `x`.
//
// The `x` and `y` are returned as fields of the Value.
//
// [Shewchuk1997], Theorem 7
constexpr auto TwoSum(const double a, const double b) -> Value {
  const double x = a + b;
  const double b_virtual = x - a;
  const double a_virtual = x - b_virtual;
  const double b_roundoff = b - b_virtual;
  const double a_roundoff = a - a_virtual;
  const double y = a_roundoff + b_roundoff;
  return {x, y};
}

// Let `a` and `b` be floating point values such that |a| >= |b|. Calculate
// non-overlapping a non-overlapping expansion `x + y` such that `a + b = x +
// y`, where `x` is an approximation to `a + b` and `y` is the roundoff error in
// the calculation of `x`.
//
// [Shewchuk1997], Theorem 6.
constexpr auto FastTwoSum(const double a, const double b) -> Value {
  // TODO(sergey): Adding an assert for |a| >= |b| triggers here for a=0 and
  // b=1e-13. Investigate whether this is something what falls under both values
  // considered 0 by the algorithm.

  const double x = a + b;
  const double b_virtual = x - a;
  const double y = b - b_virtual;
  return {x, y};
}

// Calculate sum of a double-double value `e` and a floating-point value `b`.
NO_OPTIMIZE constexpr auto DoubleAdd(const Value& e, double b) -> Value {
  // Calculate the output sequence h_{0,1,2} = (q2.x, q2.y, q1.y). h_0 has the
  // highest magnitude.
  //   Qo <= b
  //   (Q_i, H_i) <- Two-Sum(Q_i-1, ei)
  // [Shewchuk1997] Theorem 10.
  const Value q1 = TwoSum(b, e.y);
  const Value q2 = TwoSum(q1.x, e.x);

  // The [QD2000] proposes (Figure 5) to use renormalization on the sequence of
  // sums to maintain its unique representation.
  //
  // Instead, follow the simplified algorithm, similar to adding two
  // double-double values.

  return {q2.x, AddRoundToNearestEven(q2.y, q1.y)};
}

// Calculate sum of a double-double values `a` and `b`.
constexpr auto DoubleAdd(const Value& a, const Value& b) -> Value {
  // Follows the implementation of [FPHandbook2009] Algorithm 14.3.
  //
  // The [QD2000] proposes (Figure 6 and 7) to use renormalization on the
  // sequence of sums to maintain its unique representation. A lot of other
  // double-double implementations (i.e. [FPplus]) follows the [FPHandbook2009].

  const Value s = TwoSum(a.x, b.x);
  const Value t = TwoSum(a.y, b.y);

  const double c = AddRoundToNearestEven(s.y, t.y);
  const Value v = FastTwoSum(s.x, c);
  const double w = AddRoundToNearestEven(t.x, v.y);
  const Value z = FastTwoSum(v.x, w);

  return z;
}

// The following algorithm splits a 53-bit IEEE double precision floating-point
// value `a` into a_hi and a_lo, each with 26 bits of significand, such that
// `a = a_hi + a_lo`, a_hi will contain the first 26 bits, while a_lo will
// contain the lower 26 bits.
//
// The a_hi and a_lo are returned as a double-double value (a_hi, a_lo).
//
/// [QD2000] Algorithm 5.
//  [Shewchuk1997] Theorem 17.
//
// Mark as no-optimize to work around precision loss on Raspberry Pi 4 when
// building with GCC-10 where the greenwich sidereal time test fails otherwise.
NO_OPTIMIZE constexpr auto Split(const double a) -> Value {
  const double c = 134217729 * a;  // 2^27 + 1
  const double a_big = c - a;
  const double a_hi = c - a_big;
  const double a_lo = a - a_hi;
  return {a_hi, a_lo};
}

// Compute the value and error of multiplication of two floating-point values.
//
/// [QD2000] Algorithm 6.
constexpr auto TwoProd(const double a, const double b) -> Value {
  const double p = a * b;

  const Value a_hi_lo = Split(a);
  const Value b_hi_lo = Split(b);

  // Introduce aliases with names matching the paper to ease understanding the
  // formula. Assume that optimizer will inline the access without introducing
  // the extra data copy.
  const double a_hi = a_hi_lo.x;
  const double a_lo = a_hi_lo.y;
  const double b_hi = b_hi_lo.x;
  const double b_lo = b_hi_lo.y;

  const double e =
      ((a_hi * b_hi - p) + a_hi * b_lo + a_lo * b_hi) + a_lo * b_lo;

  return {p, e};
}

// Calculate product of a double-double value `a` and a floating-point value
// `b`.
constexpr auto DoubleMultiply(const Value& a, const double b) -> Value {
  // Follow the [FPHandbook2009] Algorithm 14.4 assuming that b is a
  // double-double with low-part set to 0.
  //
  // The alternatives are [Shewchuk1997] Theorem 19 and [QD2000] Figure 8. The
  // former requires renormalization and the latter requires some other form of
  // reducing the result sequence to 2 elements.
  const Value p = TwoProd(a.x, b);
  const double pl =
      AddRoundToNearestEven(p.y, MultiplyRoundToNearestEven(a.y, b));
  const Value z = FastTwoSum(p.x, pl);

  return z;
}

// Calculate product of a double-double values `a` and `b`.
constexpr auto DoubleMultiply(const Value& a, const Value& b) -> Value {
  // Follow the [FPHandbook2009] Algorithm 14.4.
  //
  // The alternative is [QD2000] Figure 9 and [Dekker1971] Section 8 function
  // mul2. The former requires renormalization. The latter is what is used
  // (in modified manner) in the [FPplus].
  //
  // TODO(sergey): Compare which of the algorithms gives the most accurate
  // result.

  Value p = TwoProd(a.x, b.x);
  p.y = AddRoundToNearestEven(p.y, MultiplyRoundToNearestEven(a.x, b.y));
  p.y = AddRoundToNearestEven(p.y, MultiplyRoundToNearestEven(a.y, b.x));
  const Value z = FastTwoSum(p.x, p.y);

  return z;
}

// Calculate the value of double-double value `a` divided by a floating-point
// value `b`.
constexpr auto DoubleDivide(const Value& a, const double b) -> Value {
  // [QD2000] Section 3.5.

  // Approximate q0 = a0 / b0
  const double q0 = a.x / b;

  // r = a - q0 * b
  Value r = DoubleAdd(a, DoubleMultiply(Value{q0, 0}, -b));

  // Correction term q1 = r0 / b0
  const double q1 = r.x / b;

  // Update the remainder r -= q1 * b
  r = DoubleAdd(r, DoubleMultiply(Value{q1, 0}, -b));

  // Correction term q2 = r0 / b0
  const double q2 = r.x / b;

  Value result = FastTwoSum(q0, q1);
  result = DoubleAdd(result, q2);

  return result;
}

// Calculate the value of double-double value `a` divided by a double-double
// value `b`.
constexpr auto DoubleDivide(const Value& a, const Value& b) -> Value {
  // [QD2000] Section 3.5.

  const Value negative_b{-b.x, -b.y};

  // Approximate q0 = a0 / b0
  const double q0 = a.x / b.x;

  // r = a - q0 * b
  Value r = DoubleAdd(a, DoubleMultiply(Value{q0, 0}, negative_b));

  // Correction term q1 = r0 / b0
  const double q1 = r.x / b.x;

  // Update the remainder r -= q1 * b
  r = DoubleAdd(r, DoubleMultiply(Value{q1, 0}, negative_b));

  // Correction term q2 = r0 / b0
  const double q2 = r.x / b.x;

  Value result = FastTwoSum(q0, q1);
  result = DoubleAdd(result, q2);

  return result;
}

// Compare two values.
//
// Returns value less than 0 if `lhs < rhs`, greater than 0 if `lhs > rhs`, and
// zero if `lhs == rhs`.
constexpr auto Compare(const Value& lhs, const Value& rhs) -> int {
  // Relies on re-normalized state of the values after every operation. This
  // allows to compare the values element-wise.
  //
  /// [QD2000] Chapter 6.2, Comparison.
  /// [QD2000] Lemma 8.

  if (lhs.x < rhs.x) {
    return -1;
  }
  if (lhs.x > rhs.x) {
    return 1;
  }

  if (lhs.y < rhs.y) {
    return -1;
  }
  if (lhs.y > rhs.y) {
    return 1;
  }

  return 0;
}

// Simplified version of Compare() to check the exact match.
constexpr auto IsEqual(const Value& lhs, const Value& rhs) -> int {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

}  // namespace double_double_internal

////////////////////////////////////////////////////////////////////////////////
// Implementation of constructors.

constexpr DoubleDouble::DoubleDouble(const double a, const double b)
    : value_(double_double_internal::TwoSum(a, b)) {}

////////////////////////////////////////////////////////////////////////////////
// Implementation of mathematical operations.

// Negation.
constexpr auto DoubleDouble::operator-() const -> DoubleDouble {
  DoubleDouble result;
  result.value_.x = -value_.x;
  result.value_.y = -value_.y;
  return result;
}

// Addition.
constexpr auto DoubleDouble::operator+=(const double rhs) -> DoubleDouble& {
  value_ = double_double_internal::DoubleAdd(value_, rhs);
  return *this;
}
constexpr auto DoubleDouble::operator+=(const DoubleDouble& rhs)
    -> DoubleDouble {
  value_ = double_double_internal::DoubleAdd(value_, rhs.value_);
  return *this;
}

// Subtraction.
constexpr auto DoubleDouble::operator-=(const double rhs) -> DoubleDouble& {
  value_ = double_double_internal::DoubleAdd(value_, -rhs);
  return *this;
}
constexpr auto DoubleDouble::operator-=(const DoubleDouble& rhs)
    -> DoubleDouble {
  value_ =
      double_double_internal::DoubleAdd(value_, {-rhs.value_.x, -rhs.value_.y});
  return *this;
}

// Multiplication.
constexpr auto DoubleDouble::operator*=(double rhs) -> DoubleDouble {
  value_ = double_double_internal::DoubleMultiply(value_, rhs);
  return *this;
}
constexpr auto DoubleDouble::operator*=(const DoubleDouble& rhs)
    -> DoubleDouble {
  value_ = double_double_internal::DoubleMultiply(value_, rhs.value_);
  return *this;
}

// Division.
constexpr auto DoubleDouble::operator/=(double rhs) -> DoubleDouble {
  value_ = double_double_internal::DoubleDivide(value_, rhs);
  return *this;
}
constexpr auto DoubleDouble::operator/=(const DoubleDouble& rhs)
    -> DoubleDouble {
  value_ = double_double_internal::DoubleDivide(value_, rhs.value_);
  return *this;
}

// Comparison.
constexpr auto operator<=>(const DoubleDouble& lhs, const DoubleDouble& rhs) {
  return double_double_internal::Compare(lhs.value_, rhs.value_) <=> 0;
}
constexpr auto operator==(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> bool {
  return double_double_internal::IsEqual(lhs.value_, rhs.value_);
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of non-member functions.

// Addition.
constexpr auto operator+(const DoubleDouble& lhs, const double rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result += rhs;
  return result;
}
constexpr auto operator+(const double lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{rhs};
  result += lhs;
  return result;
}
constexpr auto operator+(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result += rhs;
  return result;
}

// Subtraction.
constexpr auto operator-(const DoubleDouble& lhs, const double rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result -= rhs;
  return result;
}
constexpr auto operator-(const double lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result -= rhs;
  return result;
}
constexpr auto operator-(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result -= rhs;
  return result;
}

// Multiplication.
constexpr auto operator*(const DoubleDouble& lhs, const double rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result *= rhs;
  return result;
}
constexpr auto operator*(const double lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{rhs};
  result *= lhs;
  return result;
}
constexpr auto operator*(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result *= rhs;
  return result;
}

// Division.
constexpr auto operator/(const DoubleDouble& lhs, const double rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result /= rhs;
  return result;
}
constexpr auto operator/(const double lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result /= rhs;
  return result;
}
constexpr auto operator/(const DoubleDouble& lhs, const DoubleDouble& rhs)
    -> DoubleDouble {
  DoubleDouble result{lhs};
  result /= rhs;
  return result;
}

inline auto Abs(const DoubleDouble& arg) -> DoubleDouble {
  return arg < DoubleDouble(0) ? -arg : arg;
}

inline auto Trunc(const DoubleDouble& x) -> DoubleDouble {
  const double a = Trunc(x.GetHi());
  const double remainder = x.GetHi() - a + x.GetLo();

  return DoubleDouble(a, Trunc(remainder));
}

inline auto operator<<(std::ostream& os, const DoubleDouble& value)
    -> std::ostream& {
  // The maximum number of decimal digits printed.
  // Trailing zeros are omitted.
  constexpr int64_t kMaxNumDecimalDigits = 14;

  // Print the integer part.
  const int64_t integer = double(Trunc(value));
  os << integer;

  // Print the decimal separator.
  os << '.';

  // The fractional part, without sign.
  const DoubleDouble fractional = Abs(value - integer);

  // Fixed precision representation of the fractional part.
  int64_t multiplier = Pow(10.0, double(kMaxNumDecimalDigits));
  int64_t current_fractional = double(fractional * multiplier + 0.5);

  for (int i = 0; i < kMaxNumDecimalDigits; ++i) {
    multiplier /= 10;

    const int digit = current_fractional / multiplier;
    os << digit;

    current_fractional %= multiplier;
    if (current_fractional == 0) {
      break;
    }
  }

  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
