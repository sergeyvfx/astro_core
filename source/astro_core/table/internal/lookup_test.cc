// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/table/lookup.h"

#include <array>

#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(table, LookupFloorRow) {
  struct Row {
    ModifiedJulianDate key;
    int value;
  };

  auto rows = std::to_array<Row>({
      {ModifiedJulianDate(10.0), 100},
      {ModifiedJulianDate(20.0), 200},
      {ModifiedJulianDate(30.0), 300},
  });

  // Create alias so that the function can be used directly from the macro.
  const auto Lookup = [&rows](const ModifiedJulianDate& key) -> const Row& {
    return LookupFloorRow<&Row::key>(rows, key);
  };

  // Key prior to the first row of the table.
  EXPECT_EQ(Lookup(ModifiedJulianDate(0.0)).value, 100);

  // Key past to the first row of the table.
  EXPECT_EQ(Lookup(ModifiedJulianDate(50.0)).value, 300);

  // Keys at the exact row values.
  EXPECT_EQ(Lookup(ModifiedJulianDate(10.0)).value, 100);
  EXPECT_EQ(Lookup(ModifiedJulianDate(20.0)).value, 200);
  EXPECT_EQ(Lookup(ModifiedJulianDate(30.0)).value, 300);

  // Key in between of two rows.
  EXPECT_EQ(Lookup(ModifiedJulianDate(25.0)).value, 200);
}

TEST(table, LookupFloor) {
  struct Row {
    ModifiedJulianDate key;
    int value;
  };

  auto rows = std::to_array<Row>({
      {ModifiedJulianDate(10.0), 100},
      {ModifiedJulianDate(20.0), 200},
      {ModifiedJulianDate(30.0), 300},
  });

  // Create alias so that the function can be used directly from the macro.
  const auto Lookup = [&rows](const ModifiedJulianDate& key) -> int {
    return LookupFloor<&Row::key, &Row::value>(rows, key);
  };

  // Key prior to the first row of the table.
  EXPECT_EQ(Lookup(ModifiedJulianDate(0.0)), 100);

  // Key past to the first row of the table.
  EXPECT_EQ(Lookup(ModifiedJulianDate(50.0)), 300);

  // Keys at the exact row values.
  EXPECT_EQ(Lookup(ModifiedJulianDate(10.0)), 100);
  EXPECT_EQ(Lookup(ModifiedJulianDate(20.0)), 200);
  EXPECT_EQ(Lookup(ModifiedJulianDate(30.0)), 300);

  // Key in between of two rows.
  EXPECT_EQ(Lookup(ModifiedJulianDate(25.0)), 200);
}

TEST(table, LinearInterpolate) {
  struct Row {
    ModifiedJulianDate key;
    double value;
  };

  auto rows = std::to_array<Row>({
      {ModifiedJulianDate(10.0), 100},
      {ModifiedJulianDate(20.0), 200},
      {ModifiedJulianDate(30.0), 300},
  });

  // Create alias so that the function can be used directly from the macro.
  const auto Lookup = [&rows](const ModifiedJulianDate& key) -> double {
    return LinearInterpolate<&Row::key, &Row::value, DoubleDouble>(rows, key);
  };

  // Key prior to the first row of the table.
  EXPECT_EQ(Lookup(ModifiedJulianDate(0.0)), 100);

  // Key past to the first row of the table.
  EXPECT_EQ(Lookup(ModifiedJulianDate(50.0)), 300);

  // Keys at the exact row values.
  EXPECT_EQ(Lookup(ModifiedJulianDate(10.0)), 100);
  EXPECT_EQ(Lookup(ModifiedJulianDate(20.0)), 200);
  EXPECT_EQ(Lookup(ModifiedJulianDate(30.0)), 300);

  // Key in between of two rows.
  EXPECT_NEAR(Lookup(ModifiedJulianDate(21.0)), 210, 1e-12);
  EXPECT_NEAR(Lookup(ModifiedJulianDate(25.0)), 250, 1e-12);
  EXPECT_NEAR(Lookup(ModifiedJulianDate(29.0)), 290, 1e-12);
}

}  // namespace astro_core
