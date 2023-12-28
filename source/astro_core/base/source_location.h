// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// Implementation of C++20 source_location for compilers and platforms which
// do not yet have native implementation.

#pragma once

#include <version>

#include "astro_core/version/version.h"

// The Clang 15 has the following issue:
//
//   std::source_location line number wrong when used as default argument value
//   https://github.com/llvm/llvm-project/issues/56379
//
// It was fixed in Clang 16, and the fix has not been ported to the 15.
#if defined(__cpp_lib_source_location) &&                                      \
    !(defined(__clang_major__) && (__clang_major__ <= 15))
#  include <source_location>
namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

using source_location = std::source_location;
}
}  // namespace astro_core

#else

#  include <cstdint>

#  include "astro_core/base/build_config.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

struct source_location {
  // Constructs a source_location object of unspecified value.
  constexpr source_location() noexcept = default;

  source_location(const source_location& other) = default;
  source_location(source_location&& other) noexcept = default;

  static constexpr auto current(
#  if COMPILER_CLANG || COMPILER_MSVC
      const uint32_t line = __builtin_LINE(),
      const uint32_t column = __builtin_COLUMN(),
      const char* file_name = __builtin_FILE(),
      const char* function_name = __builtin_FUNCTION()
#  elif COMPILER_GCC
      const uint32_t line = __builtin_LINE(),
      const uint32_t column = 0,
      const char* file_name = __builtin_FILE(),
      const char* function_name = __builtin_FUNCTION()
#  else
      const uint32_t line = 0,
      const uint32_t column = 0,
      const char* file_name = "unknown",
      const char* function_name = "unknown"
#  endif
          ) noexcept -> source_location {
    source_location location;
    location.line_ = line;
    location.column_ = column;
    location.function_name_ = function_name;
    location.file_name_ = file_name;
    return location;
  }

  // Return the line number represented by this object.
  constexpr auto line() const noexcept -> uint32_t { return line_; }

  // Return the column number represented by this object.
  constexpr auto column() const noexcept -> uint32_t { return column_; }

  // Return the file name represented by this object.
  constexpr auto file_name() const noexcept -> const char* {
    return file_name_;
  }

  // Return the name of the function represented by this object, if any.
  constexpr auto function_name() const noexcept -> const char* {
    return function_name_;
  }

 private:
  uint32_t line_{0};
  uint32_t column_{0};
  const char* file_name_{""};
  const char* function_name_{""};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
#endif
