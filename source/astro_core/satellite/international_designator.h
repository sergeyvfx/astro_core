// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// The International Designator, also known as COSPAR ID, is an international
// identifier assigned to artificial objects in space.
//
// It consists of the launch year, a three-digit incrementing launch number of
// that year and up to a three-letter code representing the sequential
// identifier of a piece in a launch.
//
// References:
//   International Designator
//   https://en.wikipedia.org/wiki/International_Designator

#pragma once

#include <cassert>
#include <ostream>
#include <span>
#include <string>
#include <string_view>

#include "astro_core/base/error.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class InternationalDesignator {
 public:
  InternationalDesignator() = default;

  // Construct the designator from the given fields.
  //
  // Only 3 least significant digits of the launch number and only first 3
  // characters of a piece are used from the values.
  inline InternationalDesignator(const int year,
                                 const int number,
                                 const std::string_view piece) {
    SetYear(year);
    SetNumber(number);
    SetPiece(piece);
  }

  // Access the year of the launch.
  // Setter only uses the 4 least significant digits.
  constexpr auto GetYear() const -> int { return year_; }
  constexpr void SetYear(const int year) { year_ = year % 10000; }

  // Acces the launch year.
  // Setter only uses the 3 least significant digits.
  constexpr auto GetNumber() const -> int { return number_; }
  constexpr void SetNumber(const int number) { number_ = number % 1000; }

  // Access the piece of the launch.
  // Setter only uses the first 3 characters.
  constexpr auto GetPiece() const -> std::string_view { return piece_data_; }
  inline void SetPiece(const std::string_view piece) {
    const std::string_view clipped_piece = piece.substr(0, 3);
    clipped_piece.copy(piece_data_, clipped_piece.size());
  }

  // Calculate size of a buffer needed to represent this identifier in a string.
  // This does not include the null-terminator.
  auto GetBufferSize() const -> size_t {
    // 4 for the year, 1 for the dash, 3 for the number and the actual length of
    // the piece.
    return 4 + 1 + 3 + GetPiece().size();
  }

  // Canonical string representation of the designator:
  //   <Year>-<Launch Number><Piece>
  inline auto AsString() const -> std::string {
    std::string result;
    ToString(result);
    return result;
  }

  // Canonical string representation stored in the given buffer.
  //
  // The buffer is to be at least GetBufferSize() characters big. The buffer can
  // be bigger. Only year, dash, number, and piece are written to the buffer.
  // No null pointer is written to the buffer.
  //
  // Returns the error code of conversion:
  //  - Error::kOk if the designator has been fully written to the buffer
  //  - Error::kResourceExhausted if the buffer is too small to hold the
  //    designator.
  inline auto ToBuffer(std::span<char> buffer) const -> Error {
    if (buffer.size() < GetBufferSize()) {
      return Error::kResourceExhausted;
    }
    return ToBufferUnchecked(buffer);
  }

  // Canonical string representation stored in the given buffer.
  //
  // The StringType is to implement std::string-style of resize() and a
  // conversion to a span of char. This function does not write the  null
  // terminator: the string type is expected to take care of the termination in
  // its resize() method.
  //
  // The type is to support allocation of GetBufferSize() characters.
  template <class StringType>
  inline void ToString(StringType& str) const {
    str.resize(GetBufferSize());
    ToBuffer(str);
  }

 private:
  // The actual implementation of conversion into a buffer which assumes that
  // the buffer is big enough and does not perform extra checks.
  auto ToBufferUnchecked(std::span<char> buffer) const -> Error;

  // The launch year.
  int year_{0};

  // Three-digit launch number of that year.
  int number_{0};

  // Up to a three-letter code representing the sequential identifier of a piece
  // in a launch.
  // Stored as a null-terminated string.
  char piece_data_[4] = "";
};

auto operator<<(std::ostream& os, const InternationalDesignator& designator)
    -> std::ostream&;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
