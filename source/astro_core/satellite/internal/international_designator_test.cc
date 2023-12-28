// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/international_designator.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(InternationalDesignator, Construct) {
  {
    const InternationalDesignator designator(2021, 130, "A");
    EXPECT_EQ(designator.GetYear(), 2021);
    EXPECT_EQ(designator.GetNumber(), 130);
    EXPECT_EQ(designator.GetPiece(), "A");
  }

  {
    const InternationalDesignator designator(2021, 123456, "ABCDEF");
    EXPECT_EQ(designator.GetYear(), 2021);
    EXPECT_EQ(designator.GetNumber(), 456);
    EXPECT_EQ(designator.GetPiece(), "ABC");
  }
}

TEST(InternationalDesignator, AsString) {
  EXPECT_EQ(InternationalDesignator(2021, 130, "A").AsString(), "2021-130A");
  EXPECT_EQ(InternationalDesignator(2021, 13, "A").AsString(), "2021-013A");
  EXPECT_EQ(InternationalDesignator(2021, 1, "A").AsString(), "2021-001A");
}

TEST(InternationalDesignator, ToBuffer) {
  char buffer[64] = "____________";

  EXPECT_EQ(InternationalDesignator(2021, 13, "A").ToBuffer(buffer),
            Error::kOk);
  EXPECT_EQ(std::string(buffer), "2021-013A___");
}

}  // namespace astro_core
