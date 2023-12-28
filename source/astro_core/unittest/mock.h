// Copyright (c) 2021 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

// NOTE: Needs to be before include of GTest/GMock.
#include "astro_core/unittest/internal/test-internal.h"

#include <gmock/gmock.h>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace testing {

// A matcher which returns true when an absolute difference between an actual
// and expected values is smaller than the given precision.
//
// Example:
//   EXPECT_THAT(Complex(2, 3),
//               NearUsingAbsDifferenceMetric(Complex(2, 3), 1e-6f));
//
// Clang-Tidy wrongly parses this as a function.
// NOLINTNEXTLINE(modernize-use-trailing-return-type)
MATCHER_P2(NearUsingAbsDifferenceMetric, expected, precision, "") {
  const auto abs_difference = Abs(arg - expected);
  if (abs_difference < precision) {
    return true;
  }
  *result_listener << "whose absolute difference is " << abs_difference;
  return false;
}

}  // namespace testing

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
