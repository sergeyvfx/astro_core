// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/international_designator.h"

#include <cassert>

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

// Convert the number to string using the fixed width notation, padding the
// number with zeros on the left. The result is stored in the buffer without
// null-terminator added.
void NumberToBufferFixedWidth(const int number, const std::span<char> buffer) {
  assert(!buffer.empty());

  const size_t width = buffer.size();
  int current_number = number;
  for (size_t i = 0; i < width; ++i) {
    buffer[width - i - 1] = '0' + (current_number % 10);
    current_number /= 10;
  }
}

}  // namespace

auto InternationalDesignator::ToBufferUnchecked(
    const std::span<char> buffer) const -> Error {
  assert(buffer.size() >= GetBufferSize());

  NumberToBufferFixedWidth(year_, buffer.subspan<0, 4>());

  buffer[4] = '-';

  NumberToBufferFixedWidth(number_, buffer.subspan<5, 3>());

  const std::string_view piece = GetPiece();
  piece.copy(buffer.data() + 8, piece.size());

  return Error::kOk;
}

auto operator<<(std::ostream& os, const InternationalDesignator& designator)
    -> std::ostream& {
  os << designator.AsString();
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
