// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/math/math.h"

#include <array>

#include "astro_core/base/algorithm.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(math, Modulo) {
  // Test case os based on https://stackoverflow.com/a/67098028
  const auto kArguments = std::to_array<double>({-10.9,
                                                 -10.5,
                                                 -10.4,
                                                 -0.9,
                                                 -0.5,
                                                 -0.1,
                                                 0.0,
                                                 0.1,
                                                 0.5,
                                                 0.9,
                                                 10.4,
                                                 10.5,
                                                 10.9});

  for (const double a : kArguments) {
    EXPECT_NEAR(Modulo<double>(a, 3.2), std::fmod(a, 3.2), 1e-12);
    EXPECT_NEAR(Modulo<double>(a, -3.2), std::fmod(a, -3.2), 1e-12);
  }
}

TEST(math, Clamp) {
  EXPECT_EQ(Clamp(1.1f, 1.0f, 2.0f), 1.1f);
  EXPECT_EQ(Clamp(0.1f, 1.0f, 2.0f), 1.0f);
  EXPECT_EQ(Clamp(2.1f, 1.0f, 2.0f), 2.0f);
}

TEST(math, Saturate) {
  EXPECT_EQ(Saturate(0.1f), 0.1f);
  EXPECT_EQ(Saturate(-0.1f), 0.0f);
  EXPECT_EQ(Saturate(1.1f), 1.0f);
}

TEST(math, Sign) {
  EXPECT_EQ(Sign(1), 1);
  EXPECT_EQ(Sign(12), 1);

  EXPECT_EQ(Sign(-1), -1);
  EXPECT_EQ(Sign(-12), -1);

  EXPECT_EQ(Sign(0), 1);
}

TEST(math, RoundHalfToEven) {
  EXPECT_EQ(RoundHalfToEven(0.0), 0.0);

  EXPECT_EQ(RoundHalfToEven(10.0), 10.0);
  EXPECT_EQ(RoundHalfToEven(10.4), 10.0);
  EXPECT_EQ(RoundHalfToEven(10.5), 10.0);
  EXPECT_EQ(RoundHalfToEven(10.6), 11.0);

  EXPECT_EQ(RoundHalfToEven(11.0), 11.0);
  EXPECT_EQ(RoundHalfToEven(11.4), 11.0);
  EXPECT_EQ(RoundHalfToEven(11.5), 12.0);
  EXPECT_EQ(RoundHalfToEven(11.6), 12.0);

  EXPECT_EQ(RoundHalfToEven(-10.0), -10.0);
  EXPECT_EQ(RoundHalfToEven(-10.4), -10.0);
  EXPECT_EQ(RoundHalfToEven(-10.5), -10.0);
  EXPECT_EQ(RoundHalfToEven(-10.6), -11.0);

  EXPECT_EQ(RoundHalfToEven(-11.0), -11.0);
  EXPECT_EQ(RoundHalfToEven(-11.4), -11.0);
  EXPECT_EQ(RoundHalfToEven(-11.5), -12.0);
  EXPECT_EQ(RoundHalfToEven(-11.6), -12.0);
}

TEST(math, DegreesToRadians) {
  EXPECT_NEAR(DegreesToRadians(60.0), 1.0471975511965976, 1e-12);
}

TEST(math, RadiansToDegrees) {
  EXPECT_NEAR(RadiansToDegrees(1.0471975511965976), 60.0, 1e-12);
}

TEST(math, ArcsecToRadians) {
  EXPECT_NEAR(ArcsecToRadians(30.0), 0.0001454441043328608, 1e-12);
}

TEST(math, RadiansToArcsec) {
  EXPECT_NEAR(RadiansToArcsec(0.0001454441043328608), 30.0, 1e-12);
}

TEST(math, ReduceDegrees) {
  EXPECT_NEAR(ReduceDegrees(30.0), 30.0, 1e-12);
  EXPECT_NEAR(ReduceDegrees(180.0), 180.0, 1e-12);

  EXPECT_NEAR(ReduceDegrees(-30.0), -30.0, 1e-12);
  EXPECT_NEAR(ReduceDegrees(-180.0), -180.0, 1e-12);

  EXPECT_NEAR(ReduceDegrees(360.0), 0.0, 1e-12);
  EXPECT_NEAR(ReduceDegrees(400.0), 40.0, 1e-12);
}

TEST(math, ReduceRadians) {
  EXPECT_NEAR(ReduceRadians(0.1), 0.1, 1e-12);
  EXPECT_NEAR(ReduceRadians(constants::pi), constants::pi, 1e-12);

  EXPECT_NEAR(ReduceRadians(-0.1), -0.1, 1e-12);
  EXPECT_NEAR(ReduceRadians(-constants::pi), -constants::pi, 1e-12);

  EXPECT_NEAR(ReduceRadians(2 * constants::pi), 0.0, 1e-12);
  EXPECT_NEAR(ReduceRadians(2 * constants::pi + 0.1), 0.1, 1e-12);
}

TEST(math, ReduceArcsec) {
  EXPECT_NEAR(ReduceArcsec(30.0), 30.0, 1e-12);
  EXPECT_NEAR(ReduceArcsec(180.0 * 3600), 180.0 * 3600, 1e-12);

  EXPECT_NEAR(ReduceArcsec(-30.0), -30.0, 1e-12);
  EXPECT_NEAR(ReduceArcsec(-180.0 * 3600), -180.0 * 3600, 1e-12);

  EXPECT_NEAR(ReduceArcsec(360.0 * 3600), 0.0, 1e-12);
  EXPECT_NEAR(ReduceArcsec(400.0 * 3600), 40.0 * 3600, 1e-12);
}

TEST(math, NormalizeRadians) {
  EXPECT_NEAR(NormalizeRadians(0.1), 0.1, 1e-12);
  EXPECT_NEAR(NormalizeRadians(constants::pi), constants::pi, 1e-12);

  EXPECT_NEAR(NormalizeRadians(-0.1), 2 * constants::pi - 0.1, 1e-12);
  EXPECT_NEAR(NormalizeRadians(-constants::pi), constants::pi, 1e-12);

  EXPECT_NEAR(NormalizeRadians(2 * constants::pi), 0.0, 1e-12);
  EXPECT_NEAR(NormalizeRadians(2 * constants::pi + 0.1), 0.1, 1e-12);

  EXPECT_NEAR(NormalizeRadians(-2 * constants::pi), 0.0, 1e-12);
  EXPECT_NEAR(NormalizeRadians(-2 * constants::pi - 0.1),
              2 * constants::pi - 0.1,
              1e-12);
}

TEST(math, RevolutionsPerDayToRadiansPerMinute) {
  EXPECT_NEAR(RevolutionsPerDayToRadiansPerMinute(60.0), 0.261799387799, 1e-12);
}

TEST(math, MultiplyAdd) {
  const double a = 2;
  const double b = 7;
  const double c = 9;

  EXPECT_NEAR(MultiplyAdd(a, b, c), 65.0, 1e-12);
  EXPECT_NEAR(MultiplyAdd(a, c, b), 65.0, 1e-12);
}

TEST(math, Lerp) {
  EXPECT_NEAR(Lerp(3.0, 5.0, 0.0), 3.0, 1e-12);
  EXPECT_NEAR(Lerp(3.0, 5.0, 0.1), 3.2, 1e-12);
  EXPECT_NEAR(Lerp(3.0, 5.0, 0.9), 4.8, 1e-12);
  EXPECT_NEAR(Lerp(3.0, 5.0, 1.0), 5.0, 1e-12);
}

TEST(math, SinCos) {
  constexpr int kNumSteps = 3600;

  double max_error = 0;

  for (int i = 0; i < kNumSteps; ++i) {
    const double arg = 2 * constants::pi / kNumSteps * double(i);

    double sine;
    double cosine;
    SinCos(arg, sine, cosine);

    max_error = Max(max_error, Abs(sine - Sin(arg)));
    max_error = Max(max_error, Abs(cosine - Cos(arg)));
  }

  EXPECT_LT(max_error, 1e-6);
}

}  // namespace astro_core
