# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  internal/sgp4/SGP4.h
  alpha5.h
  database.h
  database_3le.h
  database_transmitter_satnogs.h
  doppler.h
  footprint.h
  international_designator.h
  orbital_state.h
  pass.h
  tle.h
  tle_parser.h
)

add_library(astro_core_satellite_obj OBJECT
  internal/sgp4/SGP4.cpp
  internal/database.cc
  internal/database_3le.cc
  internal/database_transmitter_satnogs.cc
  internal/doppler.cc
  internal/footprint.cc
  internal/international_designator.cc
  internal/orbital_state.cc
  internal/pass.cc
  internal/tle.cc
  internal/tle_parser.cc

  internal/tle_float_parser.h

  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_satellite_obj
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_satellite_obj
 PRIVATE
  astro_core_earth
  astro_core_math
  astro_core_parse
 PUBLIC
  astro_core_base
  astro_core_coordinate
  astro_core_time
  astro_core_table
)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/satellite
)

add_library(astro_core_satellite INTERFACE)
target_link_libraries(astro_core_satellite INTERFACE
    astro_core_satellite_obj $<TARGET_OBJECTS:astro_core_satellite_obj>)

remove_file_strict_compiler_flags(internal/sgp4/SGP4.cpp)

################################################################################
# Regression tests.

function(astro_core_satellite_test PRIMITIVE_NAME)
  astro_core_test(
      satellite_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_satellite external_tiny_lib)
endfunction()

astro_core_satellite_test(alpha5)
astro_core_satellite_test(database)
astro_core_satellite_test(database_3le)
astro_core_satellite_test(database_transmitter_satnogs)
astro_core_satellite_test(doppler)
astro_core_satellite_test(footprint)
astro_core_satellite_test(international_designator)
astro_core_satellite_test(pass)
astro_core_satellite_test(tle)
astro_core_satellite_test(orbital_state)
astro_core_satellite_test(tle_parser)
astro_core_satellite_test(tle_float_parser)
