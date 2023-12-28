// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// An adaptor which represents an underlying iterable object in the reverse
// order.
//
// This is a light-weight implementation of STL's std::ranges::reverse_view
// which solves a problem of some compiler, platform, and C++ std library
// combinations leading to a compilation error. For example, the example code
// from the C++ reference https://en.cppreference.com/w/cpp/ranges/reverse_view
// can not be compiled with Clang 15 on neither X86-64 nor ARM64 on Godbolt at
// the moment of writing this comment. This is also confirmed on Raspberry Pi
// OS 12 (Bookworm).
//
// Example:
//
//   const auto array = std::to_array<int>({1, 2, 3, 4, 5, 6});
//   for (const int a : reverse_view(array)) {
//     std::cout << a << " ";
//   }
//   std::cout << std::endl;
//
//   Output: 6 5 4 3 2 1

#pragma once

#include <iterator>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

template <typename T>
class reverse_view {
 public:
  explicit reverse_view(T& iterable) : iterable_{iterable} {}

  auto begin() const { return std::rbegin(iterable_); }
  auto end() const { return std::rend(iterable_); }

  // TODO(sergey): Implement the rest of the std::ranges::reverse_view.

 private:
  T& iterable_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
