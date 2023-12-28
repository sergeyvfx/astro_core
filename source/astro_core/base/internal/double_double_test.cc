// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Test the double-double numbers.
//
// The core functionality needed for the NearUsingAbsDifferenceMetric metric is
// tested using exact match comparison and comparisons of the double-double
// value casted to a floating-point value. This includes:
//
//  - Constructors
//  - Abs()
//  - Comparison
//
// The rest of the functionality is tested using NearUsingAbsDifferenceMetric
// predicate which allows to test behavior on inputs which does not fit into
// double floating-point precision without loosing precision.

#include "astro_core/base/double_double.h"

#include <limits>
#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

// The approximate age of the universe denoted in seconds.
constexpr double kUniverseAgeInSecondsScalar = 436117077000000000;

using testing::NearUsingAbsDifferenceMetric;

////////////////////////////////////////////////////////////////////////////////
// Core test.
//
// Make sure that the API needed for the gtest matcher is implemented and passes
// testing.

TEST(DoubleDouble, NearUsingAbsDifferenceMetric) {
  using testing::Not;

  EXPECT_THAT(DoubleDouble(1234.56),
              NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));

  EXPECT_THAT(DoubleDouble(1234.5678),
              NearUsingAbsDifferenceMetric(DoubleDouble(1234.5699), 1e-2));

  EXPECT_THAT(
      DoubleDouble(1234.5678),
      Not(NearUsingAbsDifferenceMetric(DoubleDouble(1234.5699), 1e-12)));
}

////////////////////////////////////////////////////////////////////////////////
// Constructors.

TEST(DoubleDouble, Construct) {
  // Construct from a single double scalar.
  {
    EXPECT_NEAR(double(DoubleDouble(0)), 0, 1e-12);
    EXPECT_NEAR(double(DoubleDouble(1234.567)), 1234.567, 1e-12);
  }

  // Construct from a two double scalars.
  {
    EXPECT_NEAR(double(DoubleDouble(0, 0)), 0, 1e-12);
    EXPECT_NEAR(double(DoubleDouble(1024.2048, 4096.8192)), 5121.024, 1e-12);
    EXPECT_NEAR(double(DoubleDouble(1024.2048, -4096.8192)), -3072.6144, 1e-12);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Negation.

TEST(DoubleDouble, Negate) {
  EXPECT_EQ(-DoubleDouble(0), DoubleDouble(0));
  EXPECT_EQ(-DoubleDouble(1024.2048), DoubleDouble(-1024.2048));
  EXPECT_EQ(-DoubleDouble(4096.8192), DoubleDouble(-4096.8192));
}

////////////////////////////////////////////////////////////////////////////////
// Unary operations.

TEST(DoubleDouble, UnaryAdd) {
  // DoubleDouble += double
  {
    DoubleDouble a(1024.2048);
    a += 4096.8192;
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(DoubleDouble(5121.024), 1e-12));
  }

  // DoubleDouble += DoubleDouble
  {
    DoubleDouble a(1024.2048);
    a += DoubleDouble(4096.8192);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(DoubleDouble(5121.024), 1e-12));
  }
}

TEST(DoubleDouble, UnarySubtract) {
  // DoubleDouble -= double
  {
    DoubleDouble a(1024.2048);
    a -= 4096.8192;
    EXPECT_THAT(a,
                NearUsingAbsDifferenceMetric(DoubleDouble(-3072.6144), 1e-12));
  }

  // DoubleDouble -= DoubleDouble
  {
    DoubleDouble a(1024.2048);
    a -= DoubleDouble(4096.8192);
    EXPECT_THAT(a,
                NearUsingAbsDifferenceMetric(DoubleDouble(-3072.6144), 1e-12));
  }
}

TEST(DoubleDouble, UnaryMultiply) {
  // DoubleDouble *= double
  {
    DoubleDouble a(1024.2048);
    a *= 10.25;
    EXPECT_THAT(a,
                NearUsingAbsDifferenceMetric(DoubleDouble(10498.0992), 1e-12));
  }

  // DoubleDouble *= DoubleDouble
  {
    DoubleDouble a(1024.2048);
    a *= DoubleDouble(10.25);
    EXPECT_THAT(a,
                NearUsingAbsDifferenceMetric(DoubleDouble(10498.0992), 1e-12));
  }
}

TEST(DoubleDouble, UnaryDivide) {
  // DoubleDouble *= double
  {
    DoubleDouble a(10498.0992);
    a /= 10.25;
    EXPECT_THAT(a,
                NearUsingAbsDifferenceMetric(DoubleDouble(1024.2048), 1e-12));
  }

  // DoubleDouble *= DoubleDouble
  {
    DoubleDouble a(10498.0992);
    a /= DoubleDouble(10.25);
    EXPECT_THAT(a,
                NearUsingAbsDifferenceMetric(DoubleDouble(1024.2048), 1e-12));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Binary operations.

TEST(DoubleDouble, BinaryAdd) {
  // DoubleDouble + double
  {
    EXPECT_THAT(DoubleDouble(1024.2048) + 4096.8192,
                NearUsingAbsDifferenceMetric(DoubleDouble(5121.024), 1e-12));
  }

  // double + DoubleDouble
  {
    EXPECT_THAT(4096.8192 + DoubleDouble(1024.2048),
                NearUsingAbsDifferenceMetric(DoubleDouble(5121.024), 1e-12));
  }

  // DoubleDouble + DoubleDouble
  {
    EXPECT_THAT(DoubleDouble(1024.2048) + DoubleDouble(4096.8192),
                NearUsingAbsDifferenceMetric(DoubleDouble(5121.024), 1e-12));
  }
}

TEST(DoubleDouble, BinarySubtract) {
  // DoubleDouble - double
  {
    EXPECT_THAT(DoubleDouble(1024.2048) - 4096.8192,
                NearUsingAbsDifferenceMetric(DoubleDouble(-3072.6144), 1e-12));
  }

  // double - DoubleDouble
  {
    EXPECT_THAT(1024.2048 - DoubleDouble(4096.8192),
                NearUsingAbsDifferenceMetric(DoubleDouble(-3072.6144), 1e-12));
  }

  // DoubleDouble - DoubleDouble
  {
    EXPECT_THAT(DoubleDouble(1024.2048) - DoubleDouble(4096.8192),
                NearUsingAbsDifferenceMetric(DoubleDouble(-3072.6144), 1e-12));
  }
}

TEST(DoubleDouble, BinaryMultiply) {
  // double * DoubleDouble
  {
    EXPECT_THAT(1024.2048 * DoubleDouble(10.25),
                NearUsingAbsDifferenceMetric(DoubleDouble(10498.0992), 1e-12));
  }

  // DoubleDouble * double
  {
    EXPECT_THAT(DoubleDouble(1024.2048) * 10.25,
                NearUsingAbsDifferenceMetric(DoubleDouble(10498.0992), 1e-12));
  }

  // DoubleDouble * DoubleDouble
  {
    EXPECT_THAT(DoubleDouble(1024.2048) * DoubleDouble(10.25),
                NearUsingAbsDifferenceMetric(DoubleDouble(10498.0992), 1e-12));
  }
}

TEST(DoubleDouble, BinaryDivide) {
  // double / DoubleDouble
  {
    EXPECT_THAT(10498.0992 / DoubleDouble(10.25),
                NearUsingAbsDifferenceMetric(DoubleDouble(1024.2048), 1e-12));
  }

  // DoubleDouble / double
  {
    EXPECT_THAT(DoubleDouble(10498.0992) / 10.25,
                NearUsingAbsDifferenceMetric(DoubleDouble(1024.2048), 1e-12));
  }

  // DoubleDouble * DoubleDouble
  {
    EXPECT_THAT(DoubleDouble(10498.0992) / DoubleDouble(10.25),
                NearUsingAbsDifferenceMetric(DoubleDouble(1024.2048), 1e-12));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Comparison.

TEST(DoubleDouble, Compare) {
  EXPECT_TRUE(DoubleDouble(0) == DoubleDouble(0));
  EXPECT_TRUE(DoubleDouble(1) != DoubleDouble(0));

  EXPECT_FALSE(DoubleDouble(0) != DoubleDouble(0));
  EXPECT_FALSE(DoubleDouble(1) == DoubleDouble(0));

  EXPECT_TRUE(DoubleDouble(0) < DoubleDouble(10));
  EXPECT_TRUE(DoubleDouble(0) <= DoubleDouble(10));
  EXPECT_FALSE(DoubleDouble(10) < DoubleDouble(0));
  EXPECT_FALSE(DoubleDouble(10) <= DoubleDouble(0));

  EXPECT_TRUE(DoubleDouble(10) > DoubleDouble(0));
  EXPECT_TRUE(DoubleDouble(10) >= DoubleDouble(0));
  EXPECT_FALSE(DoubleDouble(0) > DoubleDouble(10));
  EXPECT_FALSE(DoubleDouble(0) >= DoubleDouble(10));
}

////////////////////////////////////////////////////////////////////////////////
// Non-member functions.

TEST(DoubleDouble, Abs) {
  EXPECT_EQ(Abs(DoubleDouble(1024.2048)), DoubleDouble(1024.2048));
  EXPECT_EQ(Abs(DoubleDouble(-1024.2048)), DoubleDouble(1024.2048));
  EXPECT_EQ(Abs(-DoubleDouble(1024.2048)), DoubleDouble(1024.2048));
}

TEST(DoubleDouble, Trunc) {
  EXPECT_EQ(Trunc(DoubleDouble(1024.2048)), DoubleDouble(1024.0));
  EXPECT_EQ(Trunc(DoubleDouble(-1024.2048)), DoubleDouble(-1024.0));
}

TEST(DoubleDouble, Print) {
  {
    std::stringstream stream;
    stream << DoubleDouble(1234.0, 0.56);
    EXPECT_EQ(stream.str(), "1234.56");
  }

  {
    std::stringstream stream;
    stream << DoubleDouble(-1234.0, -0.56);
    EXPECT_EQ(stream.str(), "-1234.56");
  }
}

////////////////////////////////////////////////////////////////////////////////
// Precision.

TEST(DoubleDouble, PrecisionAdd) {
  // Add a nano-second to the age of the universe in seconds.

  // double-double + double
  {
    const DoubleDouble universe_age = kUniverseAgeInSecondsScalar;
    const double nanosecond = 1e-9;

    EXPECT_NE(universe_age + nanosecond, universe_age);
    EXPECT_GT(universe_age + nanosecond, universe_age);
    EXPECT_LT(universe_age, universe_age + nanosecond);

    EXPECT_NE(universe_age + (2 * nanosecond), universe_age + nanosecond);
    EXPECT_GT(universe_age + (2 * nanosecond), universe_age + nanosecond);

    EXPECT_NEAR(
        double((universe_age + nanosecond) - universe_age), nanosecond, 1e-12);
  }

  // double-double + double-double
  {
    const DoubleDouble universe_age = kUniverseAgeInSecondsScalar;
    const DoubleDouble nanosecond = 1e-9;

    EXPECT_NE(universe_age + nanosecond, universe_age);
    EXPECT_GT(universe_age + nanosecond, universe_age);
    EXPECT_LT(universe_age, universe_age + nanosecond);

    EXPECT_NE(universe_age + (2 * nanosecond), universe_age + nanosecond);
    EXPECT_GT(universe_age + (2 * nanosecond), universe_age + nanosecond);

    EXPECT_NEAR(
        double((universe_age + nanosecond) - universe_age), 1e-9, 1e-12);
  }
}

TEST(DoubleDouble, PrecisionMultiply) {
  // Construct double-double value from a smaller value than the universe age,
  // and multiply it to the actual age and verify the result matches.

  // double-double * double
  {
    const DoubleDouble small_age = kUniverseAgeInSecondsScalar / 1e+9;
    const DoubleDouble big_age = small_age * 1000000001;

    EXPECT_THAT(big_age,
                NearUsingAbsDifferenceMetric(
                    DoubleDouble(436117077000000000, 436117077), 1e-12));
  }

  // double-double * double-double
  {
    const DoubleDouble small_age = kUniverseAgeInSecondsScalar / 1e+9;
    const DoubleDouble big_age = small_age * DoubleDouble(1000000000, 1);

    EXPECT_THAT(big_age,
                NearUsingAbsDifferenceMetric(
                    DoubleDouble(436117077000000000, 436117077), 1e-12));
  }
}

TEST(DoubleDouble, PrecisionDivide) {
  // Effectively inverse tests of the PrecisionMultiply.

  // double-double / double
  {
    const DoubleDouble big_age = DoubleDouble(436117077000000000, 436117077);
    const DoubleDouble small_age = big_age / 1000000001;

    EXPECT_THAT(small_age,
                NearUsingAbsDifferenceMetric(DoubleDouble(436117077), 1e-12));
  }

  // double-double / double-double
  {
    const DoubleDouble big_age = DoubleDouble(436117077000000000, 436117077);
    const DoubleDouble small_age = big_age / DoubleDouble(1000000001);

    EXPECT_THAT(small_age,
                NearUsingAbsDifferenceMetric(DoubleDouble(436117077), 1e-12));
  }
}

TEST(DoubleDouble, PrecisionMultiplyDivideRoundtrip) {
  // Round trip of an Unix time to Julian date and back, using DoubleDouble and
  // scalar arithmetic.
  //
  // Verified against the numpy-based implementation:
  //   >>> import numpy as np
  //   >>> unix_time = np.float128(1667662059.0)
  //   >>> jd = unix_time / 86400 + 2440587.5
  //   >>> new_unix_time = (jd - 2440587.5) * 86400
  {
    const DoubleDouble unix_time(1667662059.0);
    const DoubleDouble jd = unix_time / 86400 + 2440587.5;
    const DoubleDouble new_unix_time = (jd - 2440587.5) * 86400;

    EXPECT_THAT(jd,
                NearUsingAbsDifferenceMetric(
                    DoubleDouble(2459889, 0.1442013888889), 1e-12));

    EXPECT_THAT(new_unix_time, NearUsingAbsDifferenceMetric(unix_time, 1e-12));
  }

  // Round trip of an Unix time to Julian date and back, using DoubleDouble
  // arithmetic.
  // Similarly to the above, verified against the numpy-based implementation.
  {
    const DoubleDouble unix_epoch_in_jd(2440587.5);
    const DoubleDouble num_seconds_in_day(86400);

    const DoubleDouble unix_time(1667662059.0);
    const DoubleDouble jd = unix_time / num_seconds_in_day + unix_epoch_in_jd;
    const DoubleDouble new_unix_time =
        (jd - unix_epoch_in_jd) * num_seconds_in_day;

    EXPECT_THAT(jd,
                NearUsingAbsDifferenceMetric(
                    DoubleDouble(2459889, 0.1442013888889), 1e-12));

    EXPECT_THAT(new_unix_time, NearUsingAbsDifferenceMetric(unix_time, 1e-12));
  }
}

}  // namespace astro_core
