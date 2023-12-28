// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

template <class T>
constexpr auto Min(const T& a, const T& b) -> const T& {
  return (b < a) ? b : a;
}

template <class T>
constexpr auto Max(const T& a, const T& b) -> const T& {
  return (a < b) ? b : a;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
