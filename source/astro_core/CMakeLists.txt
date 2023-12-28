# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT-0

add_subdirectory(base)
add_subdirectory(body)
add_subdirectory(coordinate)
add_subdirectory(earth)
add_subdirectory(math)
add_subdirectory(numeric)
add_subdirectory(parse)
add_subdirectory(satellite)
add_subdirectory(table)
add_subdirectory(time)

if(WITH_TESTS)
  add_subdirectory(unittest)
endif()

add_subdirectory(library)
