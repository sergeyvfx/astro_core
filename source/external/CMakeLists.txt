# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT-0

# Remove strict flags from code which we don't maintain and can not fix warnings
# anyway.
remove_active_strict_compiler_flags()

add_subdirectory(tiny_lib)

if(WITH_TESTS)
  add_subdirectory(gflags)
  add_subdirectory(googletest)
endif()
