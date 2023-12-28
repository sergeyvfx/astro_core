# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# Library.

set(PUBLIC_HEADERS
  lookup.h
  paged_table.h
  shared_table.h
)

add_library(astro_core_table INTERFACE
  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_table
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_table INTERFACE
  astro_core_math
)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/table
)

################################################################################
# Regression tests.

function(astro_core_table_test PRIMITIVE_NAME)
  astro_core_test(
      table_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_table)
endfunction()

astro_core_table_test(lookup)
astro_core_table_test(shared_table)
astro_core_table_test(paged_table)
