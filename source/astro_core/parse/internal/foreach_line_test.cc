// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/parse/foreach_line.h"

#include <array>
#include <string>
#include <vector>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::ElementsAre;

TEST(ForeachLine, Empty) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("")) {
    lines.emplace_back(line);
  }

  EXPECT_TRUE(lines.empty());
}

TEST(ForeachLine, NoNewLine) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("Hello, World!")) {
    lines.emplace_back(line);
  }

  EXPECT_THAT(lines, ElementsAre("Hello, World!"));
}

TEST(ForeachLine, Basic) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("Hello\nWorld\n")) {
    lines.emplace_back(line);
  }

  EXPECT_THAT(lines, ElementsAre("Hello", "World"));
}

TEST(ForeachLine, EmptyAtTheEnd) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("Hello\nWorld\n\n")) {
    lines.emplace_back(line);
  }

  EXPECT_THAT(lines, ElementsAre("Hello", "World", ""));
}

TEST(ForeachLine, EmptyInTheMiddle) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("Hello\n\nWorld")) {
    lines.emplace_back(line);
  }

  EXPECT_THAT(lines, ElementsAre("Hello", "", "World"));
}

TEST(ForeachLine, CRLF) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("Hello\r\nWorld\r\n")) {
    lines.emplace_back(line);
  }

  EXPECT_THAT(lines, ElementsAre("Hello", "World"));
}

TEST(ForeachLine, CR) {
  std::vector<std::string> lines;
  for (const std::string_view line : ForeachLine("Hello\rWorld\r")) {
    lines.emplace_back(line);
  }

  EXPECT_THAT(lines, ElementsAre("Hello", "World"));
}

}  // namespace astro_core
