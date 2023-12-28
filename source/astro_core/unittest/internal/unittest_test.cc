// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/unittest/test.h"

#include <array>
#include <span>
#include <sstream>

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace testing {

TEST(test, PrintSpan) {
  const std::array array = std::to_array<int>({1, 2, 3, 4});
  const std::span<const int> span{array};

  std::stringstream stream;
  testing::internal::PrintTo<std::span<const int>>(span, &stream);

  EXPECT_EQ(stream.str(), "{ 1, 2, 3, 4 }");
}

}  // namespace testing

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
