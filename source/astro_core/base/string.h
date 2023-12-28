// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// String utilities.

#pragma once

#include <string_view>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Returns true of the string str starts with the given prefix using
// case-insensitive comparison.
auto CaseInsensitiveStartsWith(std::string_view str, std::string_view prefix)
    -> bool;

// Find position of string needle in the string haystack using case-insensitive
// comparison.
auto CaseInsensitiveFind(std::string_view haystack, std::string_view needle)
    -> size_t;

// Compare strings in a case-insensitive manner.
//
// Returns:
//   - 0 if the strings are equal
//   - a value less than 0 if lhs < rhs
//   - a value greater than 0 if lhs > rhs
auto CaseInsensitiveCompare(std::string_view lhs, std::string_view rhs) -> int;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
