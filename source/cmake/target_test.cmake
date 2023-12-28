# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT-0

# Utility functions for defnining regression and unit test targets.

# Define unit test target.
#
# The target is specified by the name of a test (without "_test" suffic) and the
# file name it is compiled from. The "_test" for the target suffix will be added
# automatically.
#
# Extra include and system include directories can be provided to the target
# which by passing INCLUDES SYSTEM_INCLUDES options.
#
# It is possible to pass additional linking libraries by specifying "LIBRARIES"
# argument (the target will be linked against all libraries listed after the
# "LIBRRAIES" keyword).
#
# It is possible to specify runtime command line arguments passed to the test
# executable by using "ARGUMENTS" argument and passing all desired command line
# arguments after it.
#
# Example:
#
#   astro_core_test(filter internal/filter.cc
#                   DEFINITIONS USE_INTERESTING_FEATURE
#                   INCLUDES ny/private/include
#                   LIBRARIES astro_core_time
#                   ARGUMENTS --test_srcdir ${TEST_SRCDIR})
function(astro_core_test TEST_NAME FILENAME)
  if(NOT WITH_TESTS)
    return()
  endif()

  cmake_parse_arguments(
    TEST
    ""
    ""
    "DEFINITIONS;INCLUDES;SYSTEM_INCLUDES;LIBRARIES;ARGUMENTS"
    ${ARGN}
  )

  set(target_name "astro_core_${TEST_NAME}_test")

  add_executable(${target_name} ${FILENAME})

  target_include_directories(${target_name} PRIVATE
    ${TEST_INCLUDES}
  )
  target_include_directories(${target_name} SYSTEM PRIVATE
    ${TEST_SYSTEM_INCLUDES}
  )

  target_compile_definitions(${target_name} PRIVATE
    ${TEST_DEFINITIONS}
  )

  target_link_libraries(${target_name} PRIVATE
    ${TEST_LIBRARIES}
    astro_core_test_main
    GTest::gtest
    GTest::gmock
    gflags::gflags
  )

  add_test(NAME ${target_name}
           COMMAND $<TARGET_FILE:${target_name}>
           --test_srcdir=${TEST_SRCDIR_ROOT_DIR}
           ${TEST_ARGUMENTS}
  )

  # Make sure test is created in it's own dedicated directory.
  target_set_output_directory(${target_name} ${TEST_EXECUTABLE_OUTPUT_DIR})

  # Make it easy to run test projects from IDE.
  target_set_debugger_command_arguments(${target_name}
    --test_srcdir=${TEST_SRCDIR_ROOT_DIR}
    ${TEST_ARGUMENTS}
  )
endfunction()
