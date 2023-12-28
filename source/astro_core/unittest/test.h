// Copyright (c) 2021 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <filesystem>

// NOTE: Needs to be before include of GTest/GMock.
#include "astro_core/unittest/internal/test-internal.h"

#include <gtest/gtest.h>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace testing {

using namespace ::testing;  // NOLINT

// Construct a fully qualified path for the test file.
auto TestFileAbsolutePath(const std::filesystem::path& filename)
    -> std::filesystem::path;

}  // namespace testing

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core

// If the exceptions are enabled EXPECT_THROW_OR_ABORT expects that the given
// exception is thrown during evaluation of the statement. If the exceptions are
// disabled then expects that the execution of the program is aborted.
#if defined(__cpp_exceptions) && __cpp_exceptions >= 199711L
#  define EXPECT_THROW_OR_ABORT(statement, expected_exception)                 \
    EXPECT_THROW(statement, expected_exception)
#else
#  define EXPECT_THROW_OR_ABORT(statement, expected_exception)                 \
    EXPECT_DEATH(statement, "")
#endif
