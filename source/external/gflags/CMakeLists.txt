# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT-0

set(SRC
  src/gflags.cc
  src/gflags_completions.cc
  src/gflags_reporting.cc

  src/gflags/defines.h
  src/gflags/gflags_completions.h
  src/gflags/gflags_declare.h
  src/gflags/gflags_gflags.h
  src/gflags/gflags.h
  src/config.h
  src/mutex.h
  src/util.h
)


if(WIN32)
  list(APPEND SRC
    src/windows_port.cc
    src/windows_port.h
  )
endif()

add_library(external_gflags ${SRC})

target_include_directories(external_gflags SYSTEM
 PRIVATE
  src/gflags
 PUBLIC
  src
)

target_compile_definitions(external_gflags
 PUBLIC
  GFLAGS_DLL_DEFINE_FLAG=
  GFLAGS_DLL_DECLARE_FLAG=
  GFLAGS_DLL_DECL=
)

target_link_libraries(external_gflags
  Threads::Threads
)

# Create imported library for the gflags, making it eaiser to link against.
add_library(gflags::gflags INTERFACE IMPORTED GLOBAL)
set_target_properties(gflags::gflags PROPERTIES
  INTERFACE_LINK_LIBRARIES external_gflags
)
