// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/internal/date_time_core.h"

#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_format_internal {

TEST(DateTimeMath, GetNumDaysInMonth) {
  EXPECT_EQ(GetNumDaysInMonth(1900, 2), 28);
  EXPECT_EQ(GetNumDaysInMonth(1904, 2), 29);
  EXPECT_EQ(GetNumDaysInMonth(2000, 2), 29);

  EXPECT_EQ(GetNumDaysInMonth(2000, 1), 31);
  EXPECT_EQ(GetNumDaysInMonth(2000, 3), 31);
  EXPECT_EQ(GetNumDaysInMonth(2000, 4), 30);
  EXPECT_EQ(GetNumDaysInMonth(2000, 5), 31);
  EXPECT_EQ(GetNumDaysInMonth(2000, 6), 30);
  EXPECT_EQ(GetNumDaysInMonth(2000, 7), 31);
  EXPECT_EQ(GetNumDaysInMonth(2000, 8), 31);
  EXPECT_EQ(GetNumDaysInMonth(2000, 9), 30);
  EXPECT_EQ(GetNumDaysInMonth(2000, 10), 31);
  EXPECT_EQ(GetNumDaysInMonth(2000, 11), 30);
  EXPECT_EQ(GetNumDaysInMonth(2000, 12), 31);
}

}  // namespace time_format_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
