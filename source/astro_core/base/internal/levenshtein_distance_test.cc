// Copyright (c) 2019 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/levenshtein_distance.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(base, CalculateCaseInsensitiveLevenshteinDistance) {
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("", ""),
            LevenshteinDistance());
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("Hello", ""),
            LevenshteinDistance::Deletion(5));
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("", "World!"),
            LevenshteinDistance::Insertion(6));

  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("foo", "foo"),
            LevenshteinDistance());
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("foo", "bar"),
            LevenshteinDistance::Substitution(3));
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("foo", "for"),
            LevenshteinDistance::Substitution(1));
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("Hello", "World"),
            LevenshteinDistance::Substitution(4));
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("distance", "distnce"),
            LevenshteinDistance::Deletion(1));
  EXPECT_EQ(CalculateCaseInsensitiveLevenshteinDistance("distnce", "distance"),
            LevenshteinDistance::Insertion(1));
}

}  // namespace astro_core
