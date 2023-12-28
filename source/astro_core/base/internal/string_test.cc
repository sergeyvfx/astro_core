// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/string.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(base, CaseInsensitiveStartsWith) {
  EXPECT_TRUE(CaseInsensitiveStartsWith("Abcdef", "aBC"));
  EXPECT_TRUE(CaseInsensitiveStartsWith("Abcdef", "Abc"));

  EXPECT_FALSE(CaseInsensitiveStartsWith("Abcdef", "acb"));
}

TEST(base, CaseInsensitiveFind) {
  EXPECT_EQ(CaseInsensitiveFind("Hello, World", "world"), 7);

  EXPECT_EQ(CaseInsensitiveFind("Hello, World", "missing"),
            std::string_view::npos);
}

TEST(base, CaseInsensitiveCompare) {
  EXPECT_EQ(CaseInsensitiveCompare("foo", "foo"), 0);
  EXPECT_EQ(CaseInsensitiveCompare("foo", "FOO"), 0);

  EXPECT_EQ(CaseInsensitiveCompare("foo", "bar"), 1);
  EXPECT_EQ(CaseInsensitiveCompare("bar", "foo"), -1);
}

}  // namespace astro_core
