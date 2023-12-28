// Copyright (c) 2021 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/ctype.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(base, IsDigit) {
  for (const char ch : "0123456789") {
    if (ch == '\0') {
      break;
    }
    EXPECT_TRUE(IsDigit(ch)) << "Failure for " << ch;
  }

  for (const char ch : "abcdefABCDEF") {
    if (ch == '\0') {
      break;
    }
    EXPECT_FALSE(IsDigit(ch)) << "Failure for " << ch;
  }

  EXPECT_FALSE(IsDigit('\0'));
  EXPECT_FALSE(IsDigit('z'));
  EXPECT_FALSE(IsDigit('A'));
  EXPECT_FALSE(IsDigit('-'));

  EXPECT_FALSE(IsDigit(-1));
}

TEST(base, ToLowerASCII) {
  EXPECT_EQ(ToLowerASCII('A'), 'a');
  EXPECT_EQ(ToLowerASCII('B'), 'b');

  EXPECT_EQ(ToLowerASCII('a'), 'a');
  EXPECT_EQ(ToLowerASCII('b'), 'b');

  EXPECT_EQ(ToLowerASCII('0'), '0');
  EXPECT_EQ(ToLowerASCII('1'), '1');
}

TEST(base, IsLatin) {
  EXPECT_FALSE(IsLatin('0'));

  EXPECT_TRUE(IsLatin('A'));
  EXPECT_TRUE(IsLatin('B'));

  EXPECT_TRUE(IsLatin('a'));
  EXPECT_TRUE(IsLatin('b'));
}

}  // namespace astro_core
