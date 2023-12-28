# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

add_library(astro_core
  $<TARGET_OBJECTS:astro_core_base_obj>
  $<TARGET_OBJECTS:astro_core_body_obj>
  $<TARGET_OBJECTS:astro_core_satellite_obj>
  $<TARGET_OBJECTS:astro_core_earth_obj>
  $<TARGET_OBJECTS:astro_core_time_obj>
  $<TARGET_OBJECTS:astro_core_coordinate_obj>
)

astro_core_install(TARGETS astro_core)
