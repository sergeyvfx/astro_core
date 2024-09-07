# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT

add_library(external_googlemock
  src/gmock.cc
  src/gmock-cardinalities.cc
  src/gmock-internal-utils.cc
  src/gmock-matchers.cc
  src/gmock-spec-builders.cc

  include/gmock/gmock-actions.h
  include/gmock/gmock-cardinalities.h
  include/gmock/gmock-function-mocker.h
  include/gmock/gmock-matchers.h
  include/gmock/gmock-more-actions.h
  include/gmock/gmock-more-matchers.h
  include/gmock/gmock-nice-strict.h
  include/gmock/gmock-spec-builders.h
  include/gmock/gmock.h

  include/gmock/internal/gmock-internal-utils.h
  include/gmock/internal/gmock-port.h
  include/gmock/internal/gmock-pp.h

  include/gmock/internal/custom/gmock-generated-actions.h
  include/gmock/internal/custom/gmock-matchers.h
  include/gmock/internal/custom/gmock-port.h
)

target_include_directories(external_googlemock SYSTEM
 PRIVATE
  ../googletest/include
 PUBLIC
  include
)

# Create imported library for the gmock, making it easier to link against.
add_library(GTest::gmock INTERFACE IMPORTED GLOBAL)
set_target_properties(GTest::gmock PROPERTIES
  INTERFACE_LINK_LIBRARIES external_googlemock
)
