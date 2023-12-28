// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Maidenhead Locator System (QTH Locator, IARU Locator).

#pragma once

#include <array>
#include <string_view>

#include "astro_core/coordinate/geographic.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class QTH {
 public:
  static constexpr int kMaxLength = 8;

  QTH() = default;

  // Construct QTH locator from the given encoded string.
  //
  // Will only use the the valid part of the locator.
  // The locator is parsed in a case-insensitive manner.
  explicit QTH(std::string_view locator);

  // Get encoded locator as a null-terminated string.
  auto GetLocator() const -> const char* { return locator_.data(); }

  // Calculate latitude and longitude from the locator.
  auto CalculateLatitude() const -> double;
  auto CalculateLongitude() const -> double;

 private:
  // Clear the encoded locator.
  void Clear();

  std::array<char, kMaxLength + 1> locator_{};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
