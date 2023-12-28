// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// If the exceptions are enabled throws a new exception of the given type
// passing the given expression_str to its constructor as a reason. If the
// exceptions are disabled aborts the program execution.
template <class Exception, class... Args>
[[noreturn]] inline constexpr void ThrowOrAbort(
    [[maybe_unused]] Args&&... args) {
#if defined(__cpp_exceptions) && __cpp_exceptions >= 199711L
  throw Exception(std::forward<Args>(args)...);
#else
  __builtin_abort();
#endif
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
