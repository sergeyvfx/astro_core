// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/source_location.h"

#include <string_view>

#include "astro_core/unittest/test.h"

namespace astro_core {

namespace {

static void RunTest(const source_location loc = source_location::current()) {
  EXPECT_EQ(loc.line(), 34);

  // Column access is not easy to match across different compilers.
  // EXPECT_EQ(loc.column(), 28);

  // In Clang the function_name evaluates to just the function name, but in GCC
  // it evaluates to the full function name including its namespace.
  // For example, with GCC-12 the full function_name is
  //   "void radio_core::{anonymous}::MyFunction()"
  // and with Clang 15 it is just "MyFunction"
  EXPECT_TRUE(
      (std::string(loc.function_name()) == "MyFunction") ||
      (std::string_view(loc.function_name()).ends_with("::MyFunction()")));

  EXPECT_TRUE(
      std::string_view(loc.file_name()).ends_with("source_location_test.cc"));
}

static void MyFunction() { RunTest(); }

}  // namespace

TEST(base, source_location) { MyFunction(); }

}  // namespace astro_core
