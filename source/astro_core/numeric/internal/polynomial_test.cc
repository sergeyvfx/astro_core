// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/numeric/polynomial.h"

#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace numeric_internal {

TEST(numeric, Polynomial) {
  EXPECT_EQ(Polynomial(10.0, 0.0), 0.0);
  EXPECT_NEAR(Polynomial(10.0, 1.23), 1.23, 1e-12);

  EXPECT_NEAR(Polynomial(10.0, 1.2, 3.4, 4.5), 485.2, 1e-12);
}

}  // namespace numeric_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
