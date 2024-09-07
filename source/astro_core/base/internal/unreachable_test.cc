// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/unreachable.h"

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

namespace {

void UnreachableCodeTrigger() { Unreachable(); }

}  // namespace

TEST(base, unreachable) {
  // NOTE: Skip the function name check as it is compiler-dependent.
  EXPECT_DEATH_IF_SUPPORTED(UnreachableCodeTrigger(),
                            "file unreachable_test.cc");
}

}  // namespace astro_core
