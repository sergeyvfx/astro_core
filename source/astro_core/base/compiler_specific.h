// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/base/build_config.h"

// This function-like macro takes a single identifier argument that is the name
// of a builtin function, a builtin pseudo-function (taking one or more type
// arguments), or a builtin template.
// It evaluates to 1 if the builtin is supported or 0 if not.
#if defined(__has_builtin)
#  define HAS_BUILTIN(function) __has_builtin(function)
#else
#  define HAS_BUILTIN(function) 0
#endif

// Attribute used to annotate function to disable optimizations for it.
// Useful for functions where exact execution order and constant folding might
// ruin bit-perfect mathematics.
#if COMPILER_CLANG
#  define NO_OPTIMIZE [[clang::optnone]]
#elif COMPILER_GCC
#  define NO_OPTIMIZE __attribute__((optimize("O0")))
#else
#  define NO_OPTIMIZE
#endif
