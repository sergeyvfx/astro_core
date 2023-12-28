// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// Matrix and vector classes.

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Polynomial(x, a0, a1, a2, a3, ...) evaluates the following polynomial:
//
//   a0 + a1*x + a2*x^2 + a3*x^3 ...
//
// The calculation is actually done in the reverse order:
//
//   ( (a_{n} * x + a_{n-1}) * x + ... ) + a0
//
// This is both the easiest for the implementation, but also gives the best
// accuracy for polynomials evaluation in this library.
template <class X, class A0>
inline auto Polynomial(const X /*x*/, const A0 a0) {
  return a0;
}
template <class X, class A0, class... Args>
inline auto Polynomial(const X x, const A0 a0, Args... args) {
  return Polynomial(x, args...) * x + a0;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
