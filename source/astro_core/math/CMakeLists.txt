# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

set(PUBLIC_HEADERS
  math.h
)

add_library(astro_core_math INTERFACE ${PUBLIC_HEADERS})
set_property(TARGET astro_core_math PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/math
)

function(astro_core_math_test PRIMITIVE_NAME)
  astro_core_test(
      math_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_math)
endfunction()

astro_core_math_test(math)
