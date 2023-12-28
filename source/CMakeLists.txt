# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

################################################################################
# External libraries.

add_subdirectory(external)

################################################################################
# Global settings.
# Applicable for all targets defined in the subdirectories.
#
# NOTE: Ideally this is the only place which modifies global configuration.
# For the specific targets prefer to use `target_include_directories` and
# `target_compile_definitions`.

set(INC
  .
)

set(INC_SYS
)

include_directories(${INC})
include_directories(SYSTEM ${INC_SYS})

################################################################################
# Source code folders.

if(WITH_DEVELOPER_CLANG_TIDY)
  find_package(ClangTidy 12 REQUIRED)
  set(CMAKE_C_CLANG_TIDY ${ClangTidy_EXECUTABLE})
  set(CMAKE_CXX_CLANG_TIDY ${ClangTidy_EXECUTABLE})
endif()

add_subdirectory(astro_core)
