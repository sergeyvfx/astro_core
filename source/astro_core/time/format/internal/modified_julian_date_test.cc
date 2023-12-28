// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/modified_julian_date.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

////////////////////////////////////////////////////////////////////////////////
// Non-member functions.

TEST(ModifiedJulianDate, Print) {
  std::stringstream stream;
  stream << ModifiedJulianDate(1234.0, 0.56);
  EXPECT_EQ(stream.str(), "1234.56 MJD");
}

////////////////////////////////////////////////////////////////////////////////
// Conversion.

TEST(ModifiedJulianDate, ToJulianDate) {
  using FormatTraits = TimeFormatTraits<ModifiedJulianDate>;

  EXPECT_THAT(
      FormatTraits::ToJulianDate(ModifiedJulianDate(1234.56)),
      NearUsingAbsDifferenceMetric(DoubleDouble(1234.56, 2400000.5), 1e-12));
}

TEST(ModifiedJulianDate, FromJulianDate) {
  using FormatTraits = TimeFormatTraits<ModifiedJulianDate>;

  EXPECT_THAT(DoubleDouble(FormatTraits::FromJulianDate(
                  DoubleDouble(1234.56, 2400000.5))),
              NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
}

}  // namespace astro_core
