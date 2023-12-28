// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parsers of arithmetic types from string input.

#pragma once

#include <string_view>

#include "astro_core/base/convert.h"
#include "astro_core/base/result.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

enum class ArithmeticParseError {
  kNoValue,
};

// Reading and parsing utilities which operate on an integral data types.
template <class IntType>
struct IntReader {
  using Result = astro_core::Result<IntType, ArithmeticParseError>;

  // Read signed integer from the string.
  //
  // Discards any whitespace characters until the first non-whitespace character
  // is found, then takes as many characters as possible to form a valid integer
  // number representation and converts them to an integer value.
  //
  // Returns the integer value as a result, and advances the string to the first
  // character which has not been interpreted.
  //
  // If there is no interpretable integer value after the leading whitespace a
  // result with error kNoValue is returned and the string is left unchanged.
  static auto Read(std::string_view& str) -> Result {
    const std::string_view input = str;

    const IntType value = StringToInt<IntType>(input, str);

    if (str.data() == input.data()) {
      // If the string was left as-is no characters from it has been
      // interpreted.
      return Result(ArithmeticParseError::kNoValue);
    }

    return Result(value);
  }

  // Convert an entire string to an integer value.
  //
  // Returns a valid integer value if and only if the entire string has been
  // interpreted as an integer value.
  //
  // If there are non-digit symbols, or the string is empty the kNoValue error
  // is returned.
  //
  // Trailing whitespace is not considered an error.
  static auto Convert(const std::string_view str) -> Result {
    const std::string_view input = str;
    std::string_view remainder;

    const IntType value = StringToInt<IntType>(input, remainder);

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

// Reading and parsing utilities which operate on floating point data types.
template <class RealType>
struct FloatReader {
  using Result = astro_core::Result<RealType, ArithmeticParseError>;

  // Read floating point number from the string.
  //
  // Discards any whitespace characters until the first non-whitespace character
  // is found, then takes as many characters as possible to form a valid float
  // point number representation and converts them to a floating point value.
  //
  // Returns the floating point value as a result, and advances the string to
  // the first character which has not been interpreted.
  //
  // If there is no interpretable floating point value after the leading
  // whitespace a result with error kNoValue is returned and the string is left
  // unchanged.
  static auto Read(std::string_view& str) -> Result {
    const std::string_view input = str;

    const RealType value = StringToFloat<RealType>(input, str);

    if (str.data() == input.data()) {
      // If the string was left as-is no characters from it has been
      // interpreted.
      return Result(ArithmeticParseError::kNoValue);
    }

    return Result(value);
  }

  // Convert an entire string to n floating point value.
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
    const std::string_view input = str;
    std::string_view remainder;

    const RealType value = StringToFloat<RealType>(input, remainder);

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

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
