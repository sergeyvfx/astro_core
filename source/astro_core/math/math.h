// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <cmath>

#include "astro_core/base/constants.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Computes the smallest integer value not less than arg.
inline auto Ceil(const float arg) -> long { return std::ceil(arg); }
inline auto Ceil(const double arg) -> long { return std::ceil(arg); }

// Computes the largest integer value not greater than arg.
inline auto Floor(const float arg) -> float { return std::floor(arg); }
inline auto Floor(const double arg) -> double { return std::floor(arg); }

// Computes the nearest integer value to arg (in floating-point format),
// rounding halfway cases away from zero, regardless of the current rounding
// mode.
inline auto Round(const float arg) -> float { return std::round(arg); }
inline auto Round(const double arg) -> double { return std::round(arg); }

// Computes the nearest integer not greater in magnitude than arg.
inline auto Trunc(const float arg) -> float { return std::trunc(arg); }
inline auto Trunc(const double arg) -> double { return std::trunc(arg); }

// Computes the nearest integer value to arg (in integer format), rounding
// halfway cases away from zero, regardless of the current rounding mode.
inline auto RoundToInt(const float arg) -> long { return std::lround(arg); }
inline auto RoundToInt(const double arg) -> long { return std::lround(arg); }

// Computes the principal value of the arc tangent of arg.
inline auto ArcTan(const float arg) -> float { return std::atan(arg); };
inline auto ArcTan(const double arg) -> double { return std::atan(arg); };

// Computes the arc tangent of y/x using the signs of arguments to determine
// the correct quadrant.
inline auto ArcTan2(const float y, const float x) -> float {
  return std::atan2(y, x);
};
inline auto ArcTan2(const double y, const double x) -> double {
  return std::atan2(y, x);
};

// Computes the sine and cosine of arg (measured in radians).
inline auto Sin(const float arg) -> float { return std::sin(arg); }
inline auto Sin(const double arg) -> double { return std::sin(arg); }
inline auto Cos(const float arg) -> float { return std::cos(arg); }
inline auto Cos(const double arg) -> double { return std::cos(arg); }

// Computes the principal value of the arc sine of arg.
// Returns value in the range of [-pi/2 .. pi/2].
inline auto ArcSin(const float arg) -> float { return std::asin(arg); }
inline auto ArcSin(const double arg) -> double { return std::asin(arg); }

// Computes the principal value of the arc cosine of arg.
// Returns value in the range of [0 .. pi].
inline auto ArcCos(const float arg) -> float { return std::acos(arg); }
inline auto ArcCos(const double arg) -> double { return std::acos(arg); }

// Computes e (Euler's number, 2.7182818...) raised to the given power arg.
inline auto Exp(const float arg) -> float { return std::exp(arg); }
inline auto Exp(const double arg) -> double { return std::exp(arg); }

// Computes the value of base raised to the power exp.
inline auto Pow(const float base, const float exp) -> float {
  return std::pow(base, exp);
}
inline auto Pow(const float base, const int exp) -> double {
  return std::pow(base, exp);
}
inline auto Pow(const double base, const double exp) -> double {
  return std::pow(base, exp);
}
inline auto Pow(const double base, const int exp) -> double {
  return std::pow(base, exp);
}

// Computes the square root of arg.
inline auto Sqrt(const float arg) -> float { return std::sqrt(arg); }
inline auto Sqrt(const double arg) -> double { return std::sqrt(arg); }

// Computes the natural (base e) logarithm of arg.
inline auto Log(const float arg) -> float { return std::log(arg); }
inline auto Log(const double arg) -> double { return std::log(arg); }

// Computes the binary (base-2) logarithm of arg.
inline auto Log2(const float arg) -> float { return std::log2(arg); }
inline auto Log2(const double arg) -> double { return std::log2(arg); }

// Computes the common (base-10) logarithm of arg.
inline auto Log10(const float arg) -> float { return std::log10(arg); }
inline auto Log10(const double arg) -> double { return std::log10(arg); }

// Computes the absolute value of a value arg.
inline auto Abs(const int arg) -> int { return std::abs(arg); }
inline auto Abs(const float arg) -> float { return std::abs(arg); }
inline auto Abs(const double arg) -> double { return std::abs(arg); }

// Computes the square root of the sum of the squares of x and y, without undue
// overflow or underflow at intermediate stages of the computation
inline auto Hypot(const float x, const float y) -> float {
  return std::hypot(x, y);
}
inline auto Hypot(const double x, const double y) -> double {
  return std::hypot(x, y);
}

// Computes the floating-point remainder of the division operation.
inline auto Modulo(const float x, const float y) -> float {
  return std::fmod(x, y);
}
inline auto Modulo(const double x, const double y) -> double {
  return std::fmod(x, y);
}
template <class T>
inline auto Modulo(const T& x, const T& y) -> T {
  return x - Trunc(x / y) * y;
}

// Computes the IEEE remainder of the floating point division operation x/y.
inline auto Remainder(const float x, const float y) -> float {
  return std::remainder(x, y);
}
inline auto Remainder(const double x, const double y) -> double {
  return std::remainder(x, y);
}

// Composes a floating point value with the magnitude of mag and the sign of
// sgn.
inline auto CopySign(float mag, float sgn) -> float {
  return std::copysign(mag, sgn);
}

// Clamp the input argument to the given range.
template <class T>
inline auto Clamp(const T& a, const T& min, const T& max) -> T {
  if (a <= min) {
    return min;
  }
  if (a >= max) {
    return max;
  }

  return a;
}

// Clamp the input argument to [+0.0, 1.0].
template <class T>
inline auto Saturate(const T& a) -> T {
  return Clamp(a, T(0), T(1));
}

// Value of 1 with the same sign as the given argument.
// Zero has a positive sign.
template <class T>
inline auto Sign(const T& arg) -> T {
  return arg < 0 ? T(-1) : T(1);
}

// Round the value x, using round-half-to-even tiebreaker.
template <class RealType>
inline auto RoundHalfToEven(const RealType& x) -> RealType {
  const RealType rounded = Round(x);
  const RealType difference = rounded - x;

  if ((difference != RealType(0.5)) && (difference != -RealType(0.5))) {
    return rounded;
  }

  if (Modulo(rounded, RealType(2)) == RealType(0)) {
    return rounded;
  }

  return x - difference;
}

// Convert value measured in degrees to value measured in radians.
template <class T>
inline constexpr auto DegreesToRadians(const T& value) -> T {
  return value * constants::pi / 180.0;
}

// Convert value measured in radians to value measured in degrees.
template <class T>
inline constexpr auto RadiansToDegrees(const T& value) -> T {
  return value / constants::pi * 180.0;
}

// Convert value measured in arc seconds to value measured in radians.
template <class T>
inline constexpr auto ArcsecToRadians(const T& value) -> T {
  return value * constants::pi / (180.0 * 3600.0);
}

// Convert value measured in radians to value measured arc seconds.
template <class T>
inline constexpr auto RadiansToArcsec(const T& value) -> T {
  return value * (180.0 * 3600.0) / constants::pi;
}

// Reduce angle to less than 360 degrees to avoid working with large angles.
// The result is within (-360, 360).
template <class T>
inline auto ReduceDegrees(const T& a) -> T {
  return Modulo(a, T(360));
}

// Reduce angle measured in radians to be within a full circle.
// The result is within (-2*pi, 2*pi).
template <class T>
inline auto ReduceRadians(const T& a) -> T {
  return Modulo(a, T(2) * constants::pi);
}

// Reduce angle measured in arcseconds to be within a full circle.
// The result is within (-360*3600, 360*3600).
template <class T>
inline auto ReduceArcsec(const T& a) -> T {
  return Modulo(a, T(360) * 3600);
}

// Normalize angle measured in radians to [0, 2*pi) range.
template <class T>
inline auto NormalizeRadians(const T& a) {
  const T reduced = ReduceRadians(a);
  if (reduced < T(0)) {
    return reduced + T(2) * T(constants::pi);
  }
  return reduced;
}

// Convert value measured in revolutions per day to value measured in radians
// per minute.
template <class T>
inline constexpr auto RevolutionsPerDayToRadiansPerMinute(const T& value) -> T {
  constexpr const T kNumMinutesPerDay{constants::kNumMinutesInDay};
  return value / (kNumMinutesPerDay / (T(2) * constants::pi));
}

// Multiply-add to accumulator.
//   RESULT = a + (b * c)
template <class AccumulatorType, class T1, class T2>
inline constexpr auto MultiplyAdd(const AccumulatorType& a,
                                  const T1& b,
                                  const T2& c) {
  return a + (b * c);
}

// Linear interpolation between v0 and v1.
//
// This is an imprecise method (it is not guaranteed result will be v1 when
// t = 1), but it allows to benefit from hardware's FMA instruction.
template <class ValueType, class FactorType>
inline constexpr auto Lerp(const ValueType& v0,
                           const ValueType& v1,
                           const FactorType& t) {
  return MultiplyAdd(v0, v1 - v0, t);
}

// Calculate sine and cosine of the same argument `arg`.
// Depending on a platform could be faster than calling Sin() and Cos()
// sequentially.
template <class T>
inline void SinCos(const T arg, T& sine, T& cosine) {
  sine = Sin(arg);
  cosine = Cos(arg);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
