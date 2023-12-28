// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Specialized string-to-floating-point conversion for the TLE format.
// Supports assumed decimal point format where the integral part and a decimal
// point are omitted from the string representation. Additionally, supports the
// shortened scientific notation where the "e" is omitted.

#pragma once

#include "astro_core/base/convert.h"
#include "astro_core/base/result.h"
#include "astro_core/math/math.h"
#include "astro_core/parse/arithmetic.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace tle_internal {

template <class RealType, bool kUseIntegerPart>
auto StringToImplicitFloatImpl(const std::string_view str,
                               std::string_view* remainder_ptr) -> RealType {
  const std::string_view clean_str =
      convert_internal::SkipLeadingWhitespace(str);

  const size_t size = clean_str.size();
  size_t index = 0;

  RealType sign = 1;
  if (clean_str.starts_with('-')) {
    sign = -1;
    ++index;
  } else if (clean_str.starts_with('+')) {
    sign = 1;
    ++index;
  }

  RealType value = 0;

  // Integer part.
  if constexpr (kUseIntegerPart) {
    while (index < size) {
      const char ch = clean_str[index];
      if (ch >= '0' && ch <= '9') {
        value = value * 10 + convert_internal::CharToDigit(ch);
        ++index;
      } else {
        break;
      }
    }
  }

  // Fractional part.
  // When all digits are assumed to be decimal skip the separator check (as well
  // as skip advancing the cursor past it).
  if (index < size && (!kUseIntegerPart || convert_internal::IsDecimalSeparator(
                                               clean_str[index]))) {
    if constexpr (kUseIntegerPart) {
      ++index;
    }
    RealType divider = RealType(1) / 10;
    while (index < size) {
      const char ch = clean_str[index];
      if (ch >= '0' && ch <= '9') {
        value = value + convert_internal::CharToDigit(ch) * divider;
        divider /= 10;
        ++index;
      } else {
        break;
      }
    }
  }

  // Implicit exponent.
  if (index < size && (clean_str[index] == '-' || clean_str[index] == '+')) {
    const RealType exponent_sign = (clean_str[index] == '-') ? -1 : 1;
    ++index;

    // Exponent.
    RealType exponent = 0;
    while (index < size) {
      const char ch = clean_str[index];
      if (ch >= '0' && ch <= '9') {
        exponent = exponent * 10 + convert_internal::CharToDigit(ch);
        ++index;
      } else {
        break;
      }
    }
    exponent *= exponent_sign;

    value *= Pow(RealType(10), exponent);
  }

  if (remainder_ptr != nullptr) {
    if (index) {
      *remainder_ptr = clean_str.substr(index);
    } else {
      *remainder_ptr = str;
    }
  }

  return sign * value;
}

template <class RealType>
class BaseFloatReader {
 public:
  using Result = astro_core::Result<RealType, ArithmeticParseError>;

 protected:
  // Implementation of the public conversion function.
  //
  // Invokes the StringToImplicitFloatImpl() with the given kUseIntegerPart
  // template argument, and checks it output.
  template <bool kUseIntegerPart>
  static auto ConvertImpl(const std::string_view str) -> Result {
    const std::string_view input = str;
    std::string_view remainder;

    const RealType value =
        StringToImplicitFloatImpl<RealType, kUseIntegerPart>(input, &remainder);

    if (remainder.data() == input.data()) {
      // If the string was left as-is no characters from it has been
      // interpreted.
      return Result(ArithmeticParseError::kNoValue);
    }

    // Skip trailing whitespace as it is not considered to be uninterpretable
    // part of the value.
    // Use the internal convert API to ensure same rules applies for both
    // leading and trailing whitespace.
    remainder = convert_internal::SkipLeadingWhitespace(remainder);
    if (!remainder.empty()) {
      // There is non-interpretable data in the string, can not convert the
      // entire string, so return an error.
      return Result(ArithmeticParseError::kNoValue);
    }

    return Result(value);
  }
};

template <class RealType>
class FloatReader : public BaseFloatReader<RealType> {
 public:
  using Result = typename BaseFloatReader<RealType>::Result;

  // Convert an entire string to n floating point value.
  //
  // Takes care of reading the exponent which is only denoted by a sign (and not
  // the "e" which is commonly seen in the scientific notation).
  //
  // Returns a valid floating point value if and only if the entire string has
  // been interpreted as a floating point value.
  //
  // If there are non-digit symbols, or the string is empty, or the string is
  // not a valid representation of a floating point value the kNoValue error is
  // returned.
  //
  // Trailing whitespace is not considered an error.
  static auto Convert(const std::string_view str) -> Result {
    return BaseFloatReader<RealType>::template ConvertImpl<true>(str);
  }
};

template <class RealType>
class AssumedDecimalFloatReader : public BaseFloatReader<RealType> {
 public:
  using Result = typename BaseFloatReader<RealType>::Result;

  // Convert an entire string to n floating point value, assuming that the
  // digits represent the decimal part of the number. This means that, for
  // example, "1234" is returned as 0.1234.
  //
  // Takes care of reading the exponent which is only denoted by a sign (and not
  // the "e" which is commonly seen in the scientific notation).
  // Returns a valid floating point value if and only if the entire string has
  // been interpreted as a floating point value.
  //
  // If there are non-digit symbols, or the string is empty, or the string is
  // not a valid representation of a floating point value the kNoValue error is
  // returned.
  //
  // Trailing whitespace is not considered an error.
  static auto Convert(const std::string_view str) -> Result {
    return BaseFloatReader<RealType>::template ConvertImpl<false>(str);
  }
};

}  // namespace tle_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
