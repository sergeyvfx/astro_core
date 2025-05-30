# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT

add_library(external_googletest
  src/gtest.cc
  src/gtest-death-test.cc
  src/gtest-filepath.cc
  src/gtest-matchers.cc
  src/gtest-port.cc
  src/gtest-printers.cc
  src/gtest-test-part.cc
  src/gtest-typed-test.cc

  src/gtest-internal-inl.h

  include/gtest/gtest.h
  include/gtest/gtest_pred_impl.h
  include/gtest/gtest_prod.h
  include/gtest/gtest-death-test.h
  include/gtest/gtest-matchers.h
  include/gtest/gtest-message.h
  include/gtest/gtest-param-test.h
  include/gtest/gtest-printers.h
  include/gtest/gtest-spi.h
  include/gtest/gtest-test-part.h
  include/gtest/gtest-typed-test.h
  include/gtest/internal/custom/gtest.h
  include/gtest/internal/custom/gtest-port.h
  include/gtest/internal/custom/gtest-printers.h
  include/gtest/internal/gtest-death-test-internal.h
  include/gtest/internal/gtest-filepath.h
  include/gtest/internal/gtest-internal.h
  include/gtest/internal/gtest-param-util.h
  include/gtest/internal/gtest-port.h
  include/gtest/internal/gtest-port-arch.h
  include/gtest/internal/gtest-string.h
  include/gtest/internal/gtest-type-util.h
)

target_include_directories(external_googletest SYSTEM
 PRIVATE
  .
 PUBLIC
  include
)

target_link_libraries(external_googletest
  Threads::Threads
)

# Create imported library for the gtest, making it easier to link against.
add_library(GTest::gtest INTERFACE IMPORTED GLOBAL)
set_target_properties(GTest::gtest PROPERTIES
  INTERFACE_LINK_LIBRARIES external_googletest
)
