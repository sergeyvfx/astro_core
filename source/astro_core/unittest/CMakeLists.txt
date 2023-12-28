# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

add_library(astro_core_test_main
  internal/test.cc
  internal/test_main.cc

  internal/test-internal.h

  mock.h
  test.h
)

target_link_libraries(astro_core_test_main
  GTest::gtest
  GTest::gmock
  gflags::gflags
)

astro_core_test(unittest internal/unittest_test.cc)
