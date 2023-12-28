// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/rotation.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(earth, EarthRotationAngle) {
  // >>> import erfa
  // >>> erfa.era00(2460226.0, 0.08333349765277784)
  // 3.9556680590435747
  EXPECT_NEAR(EarthRotationAngle(JulianDate(2460226.0, 0.08333349765277784)),
              3.9556680590435747,
              1e-12);
}

}  // namespace astro_core
