# Copyright (c) 2021 astro core authors
#
# SPDX-License-Identifier: MIT-0

# Platform-specific configuration for WIN32 platform and MSVC compiler.

################################################################################
# Sanitizer.

if(WITH_DEVELOPER_SANITIZER)
  message(FATAL_ERROT "Sanitizer is not supported on this platform")
endif()

################################################################################
# Default compilation flags.

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# C4018 - 'expression' : signed/unsigned mismatch.
# C4389 - 'operator' : signed/unsigned mismatch.
#
#         TODO(sergey): Ideally should be solved, but would need to ensure
#         this warnming happens in all compiles. But also might be rather
#         annoying in some places.
#
# C4127 - Conditional expression is constant.
#
#         Could be removed wqith C++17 and consexpr if().
#
# C4244 - Conversion from 'type1' to 'type2', possible loss of data.
# C4245 - Conversion from 'type1' to 'type2', signed/unsigned mismatch.
# C4267 - Conversion from 'size_t' to 'type', possible loss of data.
#
# C4310 - Cast truncates constant value.
#
# C4456 - declaration of 'identifier' hides previous local declaration
# C4457 - declaration of 'identifier' hides function parameter
# C4458 - Declaration of 'identifier' hides class member.
#
# C4996 - Your code uses a function, class member, variable,
#         or typedef that's marked deprecated.
#
#         Happens for functions like fdopen().
#         TODO(sergey); Such functions probably better be wrapped into some
#         sort of higher level helper.
string(CONCAT DEFAULT_DISABLE_WARNINGS
    "/wd4018 /wd4389 /wd4127 /wd4244 /wd4245 /wd4267 /wd4310 "
    "/wd4456 /wd4457 /wd4458 /wd4996")

set(DEFAULT_C_FLAGS "/nologo /Gd /MP /W4 ${DEFAULT_DISABLE_WARNINGS}")
set(DEFAULT_C_FLAGS_DEBUG "/MDd /ZI")
set(DEFAULT_C_FLAGS_RELEASE "/MD")
set(DEFAULT_C_FLAGS_MINSIZEREL "/MD")
set(DEFAULT_C_FLAGS_RELWITHDEBINFO "/MD /ZI")

set(DEFAULT_CXX_FLAGS "${DEFAULT_C_FLAGS} /EHsc /W4 /Zc:__cplusplus")
set(DEFAULT_CXX_FLAGS_DEBUG "${DEFAULT_C_FLAGS_DEBUG}")
set(DEFAULT_CXX_FLAGS_RELEASE "${DEFAULT_C_FLAGS_RELEASE}")
set(DEFAULT_CXX_FLAGS_MINSIZEREL "${DEFAULT_C_FLAGS_MINSIZEREL}")
set(DEFAULT_CXX_FLAGS_RELWITHDEBINFO "${DEFAULT_C_FLAGS_RELWITHDEBINFO}")

set(DEFAULT_STATIC_LINKER_FLAGS "/IGNORE:4221")

# Remove all flags which are set explicitly here or which are being
# overwritten by the ones from below.
remove_all_active_compilers_flags(
  "/MDd" "/MD" "/MP"
  "/Gr" "/Gd"
  "/Z7" "/Zi" "/ZI"
  "/EHsc")

if(WITH_DEVELOPER_STRICT)
  set(DEFAULT_C_FLAGS "${DEFAULT_C_FLAGS} /WX")
  set(DEFAULT_CXX_FLAGS "${DEFAULT_CXX_FLAGS} /WX")
endif()

################################################################################
# Collect flags which are to be added to the CFLAGS and CXXFLAGS to cancel
# affect of strict flags.

# C4100 - 'identifier' : unreferenced formal parameter
# C4189 - A variable is declared and initialized but not used.
# C4459 - declaration of 'identifier' hides global declaration
# C4477 - 'function' : format string 'string' requires an argument of type
#         'type', but variadic argument number has type 'type'
# C4701 - Potentially uninitialized local variable 'name' used
#
# TODO(sergey): Consider lowering the default warning level
add_compiler_flag(CANCEL_CC_STRICT_FLAGS "/wd4100" "/wd4189" "/wd4459"
    "/wd4477" "/wd4701")

################################################################################
# Append own defaults to the CMake flags.

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${DEFAULT_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEFAULT_CXX_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${DEFAULT_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_CXX_FLAGS_DEBUG}")

set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE} ${DEFAULT_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} ${DEFAULT_CXX_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS_MINSIZEREL
    "${CMAKE_C_FLAGS_MINSIZEREL} ${DEFAULT_C_FLAGS_MINSIZEREL}")
set(CMAKE_CXX_FLAGS_MINSIZEREL
    "${CMAKE_CXX_FLAGS_MINSIZEREL} ${DEFAULT_CXX_FLAGS_MINSIZEREL}")

set(CMAKE_C_FLAGS_RELWITHDEBINFO
    "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${DEFAULT_C_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${DEFAULT_CXX_FLAGS_RELWITHDEBINFO}")

set(CMAKE_STATIC_LINKER_FLAGS
    "${CMAKE_STATIC_LINKER_FLAGS} ${DEFAULT_STATIC_LINKER_FLAGS}")

################################################################################
# Cleanup.

unset(DEFAULT_C_FLAGS)
unset(DEFAULT_C_FLAGS_DEBUG)
unset(DEFAULT_C_FLAGS_RELEASE)
unset(DEFAULT_C_FLAGS_MINSIZEREL)
unset(DEFAULT_C_FLAGS_RELWITHDEBINFO)

unset(DEFAULT_CXX_FLAGS)
unset(DEFAULT_CXX_FLAGS_DEBUG)
unset(DEFAULT_CXX_FLAGS_RELEASE)
unset(DEFAULT_CXX_FLAGS_MINSIZEREL)
unset(DEFAULT_CXX_FLAGS_RELWITHDEBINFO)
