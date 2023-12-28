// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/julian_date.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

////////////////////////////////////////////////////////////////////////////////
// Non-member functions.

TEST(JulianDate, Print) {
  std::stringstream stream;
  stream << JulianDate(1234.0, 0.56);
  EXPECT_EQ(stream.str(), "1234.56 JD");
}

////////////////////////////////////////////////////////////////////////////////
// Conversion.

TEST(JulianDate, ToJulianDate) {
  using FormatTraits = TimeFormatTraits<JulianDate>;

  EXPECT_THAT(FormatTraits::ToJulianDate(JulianDate(1234.56)),
              NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
}

TEST(JulianDate, FromJulianDate) {
  using FormatTraits = TimeFormatTraits<JulianDate>;

  EXPECT_THAT(DoubleDouble(FormatTraits::FromJulianDate(1234.56)),
              NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
}

}  // namespace astro_core
