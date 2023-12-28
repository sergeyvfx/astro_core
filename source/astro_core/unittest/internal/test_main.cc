// Copyright (c) 2021 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/unittest/test.h"

#include <gflags/gflags.h>

DEFINE_string(test_srcdir, "", "The location of data for this test.");

auto main(int argc, char** argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  return RUN_ALL_TESTS();
}
