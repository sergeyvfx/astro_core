# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  internal/tiny_lib/tl_convert.h
  internal/tiny_lib/tl_result.h
  internal/tiny_lib/tl_static_vector.h

  internal/json/json.hpp
  internal/json/json_fwd.hpp
  internal/json.h

  algorithm.h
  build_config.h
  compiler_specific.h
  constants.h
  convert.h
  ctype.h
  double_double.h
  error.h
  exception.h
  levenshtein_distance.h
  linked_list.h
  result.h
  reverse_view.h
  static_vector.h
  string.h
  source_location.h
  unreachable.h
  unroll.h
)

add_library(astro_core_base_obj
  internal/levenshtein_distance.cc
  internal/string.cc

  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_base_obj
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/base
)

add_library(astro_core_base INTERFACE)
target_link_libraries(astro_core_base INTERFACE
    astro_core_base_obj $<TARGET_OBJECTS:astro_core_base_obj>)

function(astro_core_base_test PRIMITIVE_NAME)
  astro_core_test(
      base_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_base)
endfunction()

astro_core_base_test(ctype)
astro_core_base_test(double_double)
astro_core_base_test(levenshtein_distance)
astro_core_base_test(linked_list)
astro_core_base_test(reverse_view)
astro_core_base_test(string)
astro_core_base_test(source_location)
astro_core_base_test(unreachable)
astro_core_base_test(unroll)
