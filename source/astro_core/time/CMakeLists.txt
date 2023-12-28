# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

add_subdirectory(format)

################################################################################
# Library.

set(PUBLIC_HEADERS
  compare.h
  epoch_convert.h
  greenwich_sidereal_time.h
  scale.h
  time.h
  time_difference.h
)

add_library(astro_core_time_obj OBJECT
  internal/scale_convert.h
  internal/time.cc

  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_time_obj
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_time_obj
 PUBLIC
  astro_core_base
  astro_core_time_format
  astro_core_math
 PRIVATE
  astro_core_earth
)

add_library(astro_core_time INTERFACE)
target_link_libraries(astro_core_time INTERFACE
    astro_core_time_obj $<TARGET_OBJECTS:astro_core_time_obj>)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/time
)

################################################################################
# Regression tests.

function(astro_core_time_test PRIMITIVE_NAME)
  astro_core_test(
      time_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_time astro_core_earth_test_data)
endfunction()

astro_core_time_test(compare)
astro_core_time_test(epoch_convert)
astro_core_time_test(time)
astro_core_time_test(greenwich_sidereal_time)
