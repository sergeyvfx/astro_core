// Copyright (c) 2021 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Checks if the given character is one of the 10 decimal digits: 0123456789.
//
// It is similar to standard isdigit() but does not raise any exceptions and
// does not access any locale information.
//
// For any character outside of unsigned char range will return false.
inline constexpr auto IsDigit(const int ch) -> bool {
  return ch >= '0' && ch <= '9';
}

// Convert the primary part of ASCII to lower case.
inline constexpr auto ToLowerASCII(const int ch) -> int {
  if (ch >= 'A' && ch <= 'Z') {
    return ch - 'A' + 'a';
  }
  return ch;
}

// Check whether given character is a Latin character.
// Does case-insensitive check.
inline constexpr auto IsLatin(const char ch) -> bool {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
