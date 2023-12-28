// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/parse/foreach_word.h"

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::ElementsAre;

TEST(ForeachWord, Empty) {
  std::vector<std::string> words;
  for (const std::string_view word : ForeachWord("")) {
    words.emplace_back(word);
  }

  EXPECT_TRUE(words.empty());
}

TEST(ForeachWord, Basic) {
  std::vector<std::string> words;
  for (const std::string_view word : ForeachWord("Hello World")) {
    words.emplace_back(word);
  }

  EXPECT_THAT(words, ElementsAre("Hello", "World"));
}

TEST(ForeachWord, Punctuation) {
  std::vector<std::string> words;
  for (const std::string_view word : ForeachWord("Hello, World!")) {
    words.emplace_back(word);
  }

  EXPECT_THAT(words, ElementsAre("Hello", "World"));
}

}  // namespace astro_core
