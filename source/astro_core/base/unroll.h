// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Compile time unroll of a loop.
//
// Call the given functor kCount times without using loops.
//
// Example:
//
//   std::array<int, 5> array;
//   Unroll<5>([&](const auto i) { array[i] = i + 1; });

#pragma once

#include <type_traits>
#include <utility>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace unroll_internal {

template <class T, T... kIndices, class F>
constexpr void Unroll(std::integer_sequence<T, kIndices...>, F&& f) {
  (f(std::integral_constant<T, kIndices>{}), ...);
}

}  // namespace unroll_internal

// 1-dimensional unrolling:
//   for (int i 0; i < N ++i) f(i);
template <auto N, class F>
constexpr void Unroll(F&& f) {
  unroll_internal::Unroll(std::make_integer_sequence<decltype(N), N>{},
                          std::forward<F>(f));
}

// 1-dimensional unrolling:
//   for (int i 0; i < N ++i)
//     for (int i 0; i < M ++i)
//       f(i);
template <auto N, auto M, class F>
constexpr void Unroll(F&& f) {
  Unroll<N>([&](auto i) { Unroll<M>([&](auto j) { f(i, j); }); });
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
