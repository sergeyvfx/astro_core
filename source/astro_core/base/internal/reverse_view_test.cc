// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/reverse_view.h"

#include <array>
#include <vector>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::Eq;
using testing::Pointwise;

TEST(reverse_view, Basic) {
  const auto array = std::to_array<int>({1, 2, 3, 4, 5, 6});

  std::vector<int> visited_elements;
  for (const int a : reverse_view(array)) {
    visited_elements.push_back(a);
  }

  EXPECT_THAT(visited_elements, Pointwise(Eq(), {6, 5, 4, 3, 2, 1}));
}

}  // namespace astro_core
