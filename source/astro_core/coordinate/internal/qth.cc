// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/qth.h"

#include <cassert>

#include "astro_core/base/constants.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

struct Field {
  static auto NumZones() -> int { return 18; }

  // Check whether the given character can be interpreted as a field.
  static auto IsValid(const char ch) -> char {
    if (ch >= 'A' && ch <= 'R') {
      return true;
    }

    if (ch >= 'a' && ch <= 'z') {
      return true;
    }

    return false;
  }

  // Convert field to its canonical representation which is a capital letter "A"
  // through "R".
  // The given charater is to be a valid field character.
  static auto MakeCanonical(const char ch) -> char {
    assert(IsValid(ch));

    if (ch >= 'a' && ch <= 'z') {
      return ch - 'a' + 'A';
    }

    return ch;
  }

  // Get base-0 index of a field denoted by the character.
  // If the character is a null-terminator then the result is 0.
  // The given charater is to be a valid canonical field character.
  static auto GetIndex(const char ch) -> int {
    if (ch == '\0') {
      return 0;
    }

    assert(IsValid(ch));
    assert(MakeCanonical(ch) == ch);

    return ch - 'A';
  }
};

struct Square {
  static auto NumZones() -> int { return 10; }

  // Check whether the given character can be interpreted as a square.
  static auto IsValid(const char ch) -> char {
    if (ch >= '0' && ch <= '9') {
      return true;
    }

    return false;
  }

  // Convert square to its canonical representation which is a base-10 digit
  // from 0 to 9.
  // The given charater is to be a valid square character.
  static auto MakeCanonical(const char ch) -> char {
    assert(IsValid(ch));

    return ch;
  }

  // Get base-0 index of a square denoted by the character.
  // If the character is a null-terminator then the result is 0.
  // The given charater is to be a valid canonical square character.
  static auto GetIndex(const char ch) -> int {
    if (ch == '\0') {
      return 0;
    }

    assert(IsValid(ch));
    assert(MakeCanonical(ch) == ch);

    return ch - '0';
  }
};

struct SubSquare {
  static auto NumZones() -> int { return 24; }

  // Check whether the given character can be interpreted as a sub-square.
  static auto IsValid(const char ch) -> char {
    if (ch >= 'a' && ch <= 'x') {
      return true;
    }

    if (ch >= 'A' && ch <= 'X') {
      return true;
    }

    return false;
  }

  // Convert sub-square to its canonical representation which is a lower case
  // letter "a" through "x".
  // The given charater is to be a valid sub-square character.
  static auto MakeCanonical(const char ch) -> char {
    assert(IsValid(ch));

    if (ch >= 'A' && ch <= 'X') {
      return ch - 'A' + 'a';
    }

    return ch;
  }

  // Get base-0 index of a sub-square denoted by the character.
  // If the character is a null-terminator then the result is 0.
  // The given charater is to be a valid canonical sub-square character.
  static auto GetIndex(const char ch) -> int {
    if (ch == '\0') {
      return 0;
    }

    assert(IsValid(ch));
    assert(MakeCanonical(ch) == ch);

    return ch - 'a';
  }
};

struct ExtendedSquare {
  static auto NumZones() -> int { return 10; }

  // Check whether the given character can be interpreted as an extended square.
  static auto IsValid(const char ch) -> char {
    if (ch >= '0' && ch <= '9') {
      return true;
    }

    return false;
  }

  // Convert extended square to its canonical representation which is a base-10
  // digit from 0 to 9.
  // The given charater is to be a valid extended square character.
  static auto MakeCanonical(const char ch) -> char {
    assert(IsValid(ch));

    return ch;
  }

  // Get base-0 index of an extended square denoted by the character.
  // If the character is a null-terminator then the result is 0.
  // The given charater is to be a valid canonical extended square character.
  static auto GetIndex(const char ch) -> int {
    if (ch == '\0') {
      return 0;
    }

    assert(IsValid(ch));
    assert(MakeCanonical(ch) == ch);

    return ch - '0';
  }
};

auto LocatorToRadians(const double full_range,
                      const char field,
                      const char square,
                      const char subsquare,
                      const char extended_square) -> double {
  const double field_step = full_range / Field::NumZones();
  const double square_step = field_step / Square::NumZones();
  const double subsquare_step = square_step / SubSquare::NumZones();
  const double extended_square_step =
      subsquare_step / ExtendedSquare::NumZones();

  return Field::GetIndex(field) * field_step +
         Square::GetIndex(square) * square_step +
         SubSquare::GetIndex(subsquare) * subsquare_step +
         ExtendedSquare::GetIndex(extended_square) * extended_square_step -
         full_range / 2;
}

auto LocatorToLatitude(const char field,
                       const char square,
                       const char subsquare,
                       const char extended_square) -> double {
  return LocatorToRadians(
      constants::pi, field, square, subsquare, extended_square);
}

auto LocatorToLongitude(const char field,
                        const char square,
                        const char subsquare,
                        const char extended_square) -> double {
  return LocatorToRadians(
      constants::pi * 2, field, square, subsquare, extended_square);
}

}  // namespace

QTH::QTH(const std::string_view locator) {
  const size_t length = locator.size();

  Clear();

  // Field.
  if (length >= 2) {
    if (Field::IsValid(locator[0]) && Field::IsValid(locator[1])) {
      locator_[0] = Field::MakeCanonical(locator[0]);
      locator_[1] = Field::MakeCanonical(locator[1]);
    } else {
      return;
    }
  }

  // Square.
  if (length >= 4) {
    if (Square::IsValid(locator[2]) && Square::IsValid(locator[3])) {
      locator_[2] = Square::MakeCanonical(locator[2]);
      locator_[3] = Square::MakeCanonical(locator[3]);
    } else {
      return;
    }
  }

  // Sub-square.
  if (length >= 6) {
    if (SubSquare::IsValid(locator[4]) && SubSquare::IsValid(locator[5])) {
      locator_[4] = SubSquare::MakeCanonical(locator[4]);
      locator_[5] = SubSquare::MakeCanonical(locator[5]);
    } else {
      return;
    }
  }

  // Extended square.
  if (length >= 8) {
    if (ExtendedSquare::IsValid(locator[6]) &&
        ExtendedSquare::IsValid(locator[7])) {
      locator_[6] = ExtendedSquare::MakeCanonical(locator[6]);
      locator_[7] = ExtendedSquare::MakeCanonical(locator[7]);
    } else {
      return;
    }
  }
}

auto QTH::CalculateLatitude() const -> double {
  return LocatorToLatitude(locator_[1], locator_[3], locator_[5], locator_[7]);
}

auto QTH::CalculateLongitude() const -> double {
  return LocatorToLongitude(locator_[0], locator_[2], locator_[4], locator_[6]);
}

void QTH::Clear() { locator_.fill('\0'); }

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
