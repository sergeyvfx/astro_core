# Copyright (c) 2023 astro core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  moon.h
)

add_library(astro_core_body_obj OBJECT
  internal/moon.cc

  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_body_obj PROPERTY
             PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_body_obj
 PRIVATE
  astro_core_base
  astro_core_math
  astro_core_numeric
  astro_core_time
 PUBLIC
  astro_core_coordinate
)

add_library(astro_core_body INTERFACE)
target_link_libraries(astro_core_body INTERFACE
    astro_core_body_obj $<TARGET_OBJECTS:astro_core_body_obj>)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/coordinate
)

################################################################################
# Regression tests.

function(astro_core_body_test PRIMITIVE_NAME)
  astro_core_test(
      coordinate_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_body astro_core_earth_test_data)
endfunction()

astro_core_body_test(moon)
