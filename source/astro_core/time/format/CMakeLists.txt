# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT

################################################################################
# Library.

set(PUBLIC_HEADERS
  internal/convert.h
  internal/date_time_core.h
  internal/date_time_math.h

  traits.h

  arithmetic.h

  date_time.h
  julian_date.h
  modified_julian_date.h
  system_clock.h
  unix_time.h
  year_decimal_day.h
)

add_library(astro_core_time_format INTERFACE
  ${PUBLIC_HEADERS}
)
set_property(TARGET astro_core_time_format
             PROPERTY PUBLIC_HEADER ${PUBLIC_HEADERS})

target_link_libraries(astro_core_time_format
 INTERFACE
  astro_core_base
  astro_core_math
)

astro_core_install_with_directory(
    FILES ${PUBLIC_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/time/format
)

################################################################################
# Regression tests.

function(astro_core_time_format_test PRIMITIVE_NAME)
  astro_core_test(
      time_format_${PRIMITIVE_NAME} internal/${PRIMITIVE_NAME}_test.cc
      LIBRARIES astro_core_time_format)
endfunction()

astro_core_time_format_test(arithmetic)
astro_core_time_format_test(convert)
astro_core_time_format_test(date_time)
astro_core_time_format_test(date_time_core)
astro_core_time_format_test(date_time_math)
astro_core_time_format_test(julian_date)
astro_core_time_format_test(modified_julian_date)
astro_core_time_format_test(system_clock)
astro_core_time_format_test(unix_time)
astro_core_time_format_test(year_decimal_day)
