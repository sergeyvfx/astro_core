// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Alpha-5 numbering extension for TLE satellite catalog number.
//
// Alpha-5 is a stopgap object numbering schema from the United States Space
// Force that increases the satellite catalog’s capacity to display up to
// 339,999 objects in the GP/GP_History API classes using legacy fixed-width Two
// and Three Line Element Set (TLE/3LE) formats.//
//
// Only capital letters and numbers are used in Alpha-5. The letters “I” and “O”
// are omitted to avoid confusion with the numbers “1” and “0”.
//
//   Full Alpha-5 Numbering
//   ======================
//   A => 10    J => 18    S => 26
//   B => 11    K => 19    T => 27
//   C => 12    L => 20    U => 28
//   D => 13    M => 21    V => 29
//   E => 14    N => 22    W => 30
//   F => 15    P => 23    X => 31
//   G => 16    Q => 24    Y => 32
//   H => 17    R => 25    Z => 33
//
// Reference:
//
//   https://www.space-track.org/documentation#tle-alpha5

#pragma once

#include <cassert>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Convert number to Alpha-5 character.
// Passing numbers outside of 10 .. 33 range is undefined.
constexpr inline auto NumberToAlpha5(const int number) -> char {
  assert(number >= 10);
  assert(number <= 33);

  if (number <= 17) {
    return number - 10 + 'A';
  }

  if (number <= 22) {
    return number - 18 + 'J';
  }

  return number - 23 + 'P';
}

// Convert Alpha-5 character to an actual number.
// THe output for non-capital latin letters and characters I and O is undefined.
constexpr inline auto Alpha5ToNumber(const char ch) -> int {
  assert(ch >= 'A');
  assert(ch <= 'Z');
  assert(ch != 'O');
  assert(ch != 'I');

  if (ch <= 'H') {
    return ch - 'A' + 10;
  }

  if (ch <= 'N') {
    return ch - 'J' + 18;
  }

  return ch - 'P' + 23;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
