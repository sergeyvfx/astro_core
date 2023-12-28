// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/internal/tle_float_parser.h"

#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace tle_internal {

TEST(TLEFloatReader, Convert) {
  using FloatReader = tle_internal::FloatReader<double>;

  {
    const FloatReader::Result result = FloatReader::Convert("-.11606-4");
    EXPECT_TRUE(result.Ok());
    EXPECT_NEAR(result.GetValue(), -0.11606e-4, 1e-12);
  }

  {
    const FloatReader::Result result = FloatReader::Convert("-.11606+4");
    EXPECT_TRUE(result.Ok());
    EXPECT_NEAR(result.GetValue(), -0.11606e+4, 1e-12);
  }
}

TEST(TLEAssumedDecimalFloatReader, ConvertAssumedDecimal) {
  using AssumedDecimalFloatReader =
      tle_internal::AssumedDecimalFloatReader<double>;

  {
    const AssumedDecimalFloatReader::Result result =
        AssumedDecimalFloatReader::Convert("-11606-4");
    EXPECT_TRUE(result.Ok());
    EXPECT_NEAR(result.GetValue(), -0.11606e-4, 1e-12);
  }

  {
    const AssumedDecimalFloatReader::Result result =
        AssumedDecimalFloatReader::Convert("-11606+4");
    EXPECT_TRUE(result.Ok());
    EXPECT_NEAR(result.GetValue(), -0.11606e+4, 1e-12);
  }
}

}  // namespace tle_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
