// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/version/version.h"

#define TL_CONVERT_NAMESPACE astro_core::bundled_tiny_lib::convert
#include "astro_core/base/internal/tiny_lib/tl_convert.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Convert string to an integer value.
//
// Discards any whitespace characters until the first non-whitespace character
// is found Then the string is being interpreted while the value is
// representing a valid integral.
//
// The optional remainder will hold the un-interpreted part of the string.
// If no valid value was read from the stirng then the remainder matches the
// input string. It is possible to use the input stirng pointer as the
// remainder.
//
// Returns 0 for the empty string input, or there is no valid integer value in
// the stirng after leading whitespace.
//
// Does not perform any locale lookups so often behaves faster than the similar
// functions from the standard library.
//
// Limitations:
//   - Integer and floating point overflows are not handled.
//   - Only base-10 system.

using bundled_tiny_lib::convert::StringToInt;

// Convert string to a floating point value.
//
// Discards any whitespace characters until the first non-whitespace character
// is found, Then the string is being interpreted while the value is
// representing a valid floating point value.
//
// The optional remainder will hold the un-interpreted part of the string.
// If no valid value was read from the stirng then the remainder matches the
// input string. It is possible to use the input stirng pointer as the
// remainder.
//
// Returns 0 for the empty string input, or there is no valid floating point
// value in the stirng after leading whitespace.
//
// Does not perform any locale lookups so often behaves faster than the similar
// functions from the standard library.
//
// Limitations:
//   - Integer and floating point overflows are not handled.
//   - Only base-10 system.
//   - Scientific notation is not supported.
//   - nan/inf notation is not supported.

using bundled_tiny_lib::convert::StringToFloat;

// Convert integer value to a string and store it in the given buffer.
//
// If the buffer does not have enough space the content of it is undefined and
// the function returns false.
// Upon successful conversion true is returned.
//
// The conversion happens without accessing current system locale, and without
// any memory allocations.

using bundled_tiny_lib::convert::IntToStringBuffer;

namespace convert_internal {
using bundled_tiny_lib::convert::convert_internal::CharToDigit;
using bundled_tiny_lib::convert::convert_internal::IsDecimalSeparator;
using bundled_tiny_lib::convert::convert_internal::SkipLeadingWhitespace;
}  // namespace convert_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
