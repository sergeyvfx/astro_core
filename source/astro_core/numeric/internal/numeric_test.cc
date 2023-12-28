// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/numeric/numeric.h"

#include "astro_core/base/constants.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(numeric, VectorRotationAroundX) {
  EXPECT_THAT(VectorRotationAroundX(constants::pi / 6.0),
              Pointwise(DoubleNear(1e-5),
                        Mat3::FromRows({{1.0, 0.0, 0.0},
                                        {0.0, 0.866025, -0.5},
                                        {0.0, 0.5, 0.866025}})));
}

TEST(numeric, VectorRotationAroundY) {
  EXPECT_THAT(VectorRotationAroundY(constants::pi / 6.0),
              Pointwise(DoubleNear(1e-5),
                        Mat3::FromRows({{0.866025, 0.0, 0.5},
                                        {0.0, 1.0, 0.0},
                                        {-0.5, 0.0, 0.866025}})));
}

TEST(numeric, VectorRotationAroundZ) {
  EXPECT_THAT(VectorRotationAroundZ(constants::pi / 6.0),
              Pointwise(DoubleNear(1e-5),
                        Mat3::FromRows({{0.866025, -0.5, 0.0},
                                        {0.5, 0.866025, 0.0},
                                        {0.0, 0.0, 1.0}})));
}

TEST(numeric, AxisRotationAroundX) {
  EXPECT_THAT(AxisRotationAroundX(constants::pi / 6.0),
              Pointwise(DoubleNear(1e-5),
                        Mat3::FromRows({{1.0, 0.0, 0.0},
                                        {0.0, 0.866025, 0.5},
                                        {0.0, -0.5, 0.866025}})));
}

TEST(numeric, AxisRotationAroundY) {
  EXPECT_THAT(AxisRotationAroundY(constants::pi / 6.0),
              Pointwise(DoubleNear(1e-5),
                        Mat3::FromRows({{0.866025, 0.0, -0.5},
                                        {0.0, 1.0, 0.0},
                                        {0.5, 0.0, 0.866025}})));
}

TEST(numeric, AxisRotationAroundZ) {
  EXPECT_THAT(AxisRotationAroundZ(constants::pi / 6.0),
              Pointwise(DoubleNear(1e-5),
                        Mat3::FromRows({{0.866025, 0.5, 0.0},
                                        {-0.5, 0.866025, 0.0},
                                        {0.0, 0.0, 1.0}})));
}

}  // namespace astro_core
