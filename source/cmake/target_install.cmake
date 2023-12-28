# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT-0

include(GNUInstallDirs)

# Helper function which takes care of installing list of files into a
# destination preserving relative directory structure.
#
# Typical usage:
#
#   astro_core_install_with_directory(
#     FILES ${PUBLIC_HEADERS}
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/astro_core/component
#   )
#
function(astro_core_install_with_directory)
  cmake_parse_arguments(INSTALL
    ""
    "DESTINATION"
    "FILES"
    ${ARGN}
  )
  foreach(_file ${INSTALL_FILES})
    get_filename_component(_dir ${_file} DIRECTORY)
    install(FILES ${_file} DESTINATION ${INSTALL_DESTINATION}/${_dir})
  endforeach()
endfunction()

# A wrapper around install() which allows to override the behavior when th
# library sources are bundled into another project.
function(astro_core_install)
  install(${ARGN})
endfunction()
