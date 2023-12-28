// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/compare.h"

#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/scale.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(Time, Compare) {
  EXPECT_FALSE(TimeCompare::lt(Time(JulianDate(1000), TimeScale::kUTC),
                               Time(JulianDate(100), TimeScale::kUTC)));
  EXPECT_FALSE(TimeCompare::lt(Time(JulianDate(100), TimeScale::kUTC),
                               Time(JulianDate(100), TimeScale::kUTC)));
  EXPECT_TRUE(TimeCompare::lt(Time(JulianDate(100), TimeScale::kUTC),
                              Time(JulianDate(1000), TimeScale::kUTC)));
}

}  // namespace astro_core
