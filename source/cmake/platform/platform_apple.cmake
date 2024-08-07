# Copyright (c) 2018 astro core authors
#
# SPDX-License-Identifier: MIT-0

# Platform-specific configuration for APPLE platform.

include(platform/platform_unix_common)

set(CMAKE_MACOSX_RPATH ON)

set(DEFAULT_C_FLAGS)
set(DEFAULT_CXX_FLAGS)
set(DEFAULT_LINKER_FLAGS)

################################################################################
# Address sanitizer.

if(WITH_DEVELOPER_SANITIZER)
  set(_asan_flags "-fsanitize=address")
  add_compiler_flag(DEFAULT_C_FLAGS "${_asan_flags}")
  add_compiler_flag(DEFAULT_CXX_FLAGS "${_asan_flags}")
  # Pass -fsanitize=address to the linker so that it links against ASAN libraries
  # avoiding linker error when using Xcode generator.
  add_compiler_flag(DEFAULT_LINKER_FLAGS "${_asan_flags}")
  unset(_asan_flags)
endif()

################################################################################
# Linker and compiler flags tweaks.

add_compiler_flag(CMAKE_C_FLAGS "${DEFAULT_C_FLAGS}")
add_compiler_flag(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}")

add_compiler_flag(CMAKE_EXE_LINKER_FLAGS "${DEFAULT_LINKER_FLAGS}")
add_compiler_flag(CMAKE_SHARED_LINKER_FLAGS "${DEFAULT_LINKER_FLAGS}")
add_compiler_flag(CMAKE_MODULE_LINKER_FLAGS "${DEFAULT_LINKER_FLAGS}")

unset(DEFAULT_C_FLAGS)
unset(DEFAULT_CXX_FLAGS)
unset(DEFAULT_LINKER_FLAGS)
