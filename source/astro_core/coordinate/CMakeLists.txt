# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  cartesian.h
  spherical.h

  frame.h

  gcrf.h
  geodetic.h
  geographic.h
  horizontal.h
  itrf.h
  qth.h
  teme.h

  representation_view.h

  frame_transform.h
)

add_library(astro_core_coordinate_obj OBJECT
  internal/cartesian.cc
  internal/spherical.cc

  internal/gcrf.cc
  internal/geodetic.cc
  internal/geographic.cc
  internal/horizontal.cc
  internal/itrf.cc
  internal/qth.cc
  internal/teme.cc

  internal/frame_transform.cc

  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_coordinate_obj PROPERTY
             PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_coordinate_obj
 PUBLIC
  astro_core_base
  astro_core_math
  astro_core_numeric
  astro_core_time
 PRIVATE
  astro_core_earth
)

add_library(astro_core_coordinate INTERFACE)
target_link_libraries(astro_core_coordinate INTERFACE
    astro_core_coordinate_obj $<TARGET_OBJECTS:astro_core_coordinate_obj>)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/coordinate
)

################################################################################
# Regression tests.

function(astro_core_coordinate_test PRIMITIVE_NAME)
  astro_core_test(
      coordinate_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_coordinate astro_core_earth_test_data)
endfunction()

astro_core_coordinate_test(cartesian)
astro_core_coordinate_test(spherical)

astro_core_coordinate_test(frame)

astro_core_coordinate_test(gcrf)
astro_core_coordinate_test(geodetic)
astro_core_coordinate_test(geographic)
astro_core_coordinate_test(horizontal)
astro_core_coordinate_test(itrf)
astro_core_coordinate_test(qth)
astro_core_coordinate_test(teme)

astro_core_coordinate_test(representation_view)

astro_core_coordinate_test(frame_transform)
