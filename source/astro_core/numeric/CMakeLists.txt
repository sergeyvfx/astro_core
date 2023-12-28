# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  internal/matrix.h
  internal/vector.h

  numeric.h
  polynomial.h
)

add_library(astro_core_numeric INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET astro_core_numeric PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_numeric INTERFACE
  astro_core_math
)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/numeric
)

function(astro_core_numeric_test PRIMITIVE_NAME)
  astro_core_test(
      numeric_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_numeric)
endfunction()

astro_core_numeric_test(matrix)
astro_core_numeric_test(numeric)
astro_core_numeric_test(polynomial)
astro_core_numeric_test(vector)
