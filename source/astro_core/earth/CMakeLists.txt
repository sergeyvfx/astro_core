# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  earth.h

  nutation.h

  orientation.h
  orientation_data.h
  orientation_table.h
  orientation_iers_a.h
  orientation_iers_b.h

  precession.h
  rotation.h

  celestial_intermediate_pole.h
  geocentric_radius.h
  intermediate_rotation.h
  terrestrial_intermediate_origin.h

  leap_second.h
  leap_second_data.h
  leap_second_table.h
  leap_second_iers.h

  internal/table.h
)

add_library(astro_core_earth_obj OBJECT
  internal/intermediate_rotation.cc
  internal/nutation.cc

  internal/orientation.cc
  internal/orientation_data.cc
  internal/orientation_table.cc
  internal/orientation_iers_a.cc
  internal/orientation_iers_b.cc

  internal/precession.cc

  internal/leap_second.cc
  internal/leap_second_data.cc
  internal/leap_second_table.cc
  internal/leap_second_iers.cc

  internal/celestial_intermediate_pole.cc

  internal/nutation_arguments.cc
  internal/nutation_arguments.h

  internal/iers/tab5.2a.h
  internal/iers/tab5.2b.h
  internal/iers/tab5.2d.h
  internal/iers/tab5.3a.h
  internal/iers/tab5.3b.h

  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_earth_obj
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_earth_obj
 PUBLIC
  astro_core_base
  astro_core_numeric
  astro_core_time_format
  astro_core_math
 PRIVATE
  astro_core_parse
  astro_core_table
)

add_library(astro_core_earth INTERFACE)
target_link_libraries(astro_core_earth INTERFACE
    astro_core_earth_obj $<TARGET_OBJECTS:astro_core_earth_obj>)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/earth
)

################################################################################
# Regression tests.

add_library(astro_core_earth_test_data INTERFACE
  internal/leap_second_test_data.h
  internal/orientation_test_data.h
  internal/earth_test_data.h
)
target_link_libraries(astro_core_earth_test_data INTERFACE
  astro_core_earth
  external_tiny_lib
)

function(astro_core_earth_test PRIMITIVE_NAME)
  astro_core_test(
      earth_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_earth astro_core_earth_test_data)
endfunction()

astro_core_earth_test(celestial_intermediate_pole)
astro_core_earth_test(geocentric_radius)
astro_core_earth_test(intermediate_rotation)
astro_core_earth_test(leap_second)
astro_core_earth_test(leap_second_data)
astro_core_earth_test(leap_second_table)
astro_core_earth_test(leap_second_iers)
astro_core_earth_test(nutation)
astro_core_earth_test(nutation_arguments)
astro_core_earth_test(orientation)
astro_core_earth_test(orientation_data)
astro_core_earth_test(orientation_table)
astro_core_earth_test(orientation_iers_a)
astro_core_earth_test(orientation_iers_b)
astro_core_earth_test(precession)
astro_core_earth_test(rotation)
astro_core_earth_test(terrestrial_intermediate_origin)
