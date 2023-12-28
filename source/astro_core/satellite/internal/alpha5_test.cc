
// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/alpha5.h"

#include <string>

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(Satellite, NumberToAlpha5) {
  std::string alpha5;

  for (int i = 10; i <= 33; ++i) {
    alpha5 += NumberToAlpha5(i);
  }

  EXPECT_EQ(alpha5, "ABCDEFGHJKLMNPQRSTUVWXYZ");
}

TEST(Satellite, Alpha5ToNumber) {
  const std::string alpha5 = "ABCDEFGHJKLMNPQRSTUVWXYZ";

  for (int i = 10; i <= 33; ++i) {
    EXPECT_EQ(Alpha5ToNumber(alpha5[i - 10]), i) << i;
  }
}

}  // namespace astro_core
