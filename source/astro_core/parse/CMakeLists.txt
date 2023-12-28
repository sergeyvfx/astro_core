# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  arithmetic.h
  field_parser.h
  foreach_line.h
  foreach_word.h
)

add_library(astro_core_parse INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET astro_core_parse PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/parse
)

function(astro_core_parse_test PRIMITIVE_NAME)
  astro_core_test(
      parse_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_parse)
endfunction()

astro_core_parse_test(arithmetic)
astro_core_parse_test(field_parser)
astro_core_parse_test(foreach_line)
astro_core_parse_test(foreach_word)
