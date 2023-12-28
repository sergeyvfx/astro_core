// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/numeric/internal/vector.h"

#include <sstream>

#include "astro_core/base/double_double.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace numeric_internal {

using testing::DoubleNear;
using testing::Eq;
using testing::Pointwise;

using Vec2i = Vector<int, 2>;
using Vec3i = Vector<int, 3>;
using Vec4i = Vector<int, 4>;

using Vec2 = Vector<double, 2>;
using Vec3 = Vector<double, 3>;
using Vec4 = Vector<double, 4>;

////////////////////////////////////////////////////////////////////////////////
// Constructor.

TEST(Vector, Construct) {
  EXPECT_THAT(Vec4i(1, 2, 3, 4).Data(), Pointwise(Eq(), {1, 2, 3, 4}));

  EXPECT_THAT(Vec4i::Zero().Data(), Pointwise(Eq(), {0, 0, 0, 0}));

  EXPECT_THAT(Vec4i::Constant(3).Data(), Pointwise(Eq(), {3, 3, 3, 3}));
}

////////////////////////////////////////////////////////////////////////////////
// Shape.

TEST(Vector, Dimension) {
  EXPECT_EQ(Vec2::kStaticDimension, 2);
  EXPECT_EQ(Vec2().GetDimension(), 2);

  EXPECT_EQ(Vec3::kStaticDimension, 3);
  EXPECT_EQ(Vec3().GetDimension(), 3);
}

////////////////////////////////////////////////////////////////////////////////
// Element access.

TEST(Vector, ElementAccess) {
  const Vec4i vec(1, 2, 3, 4);
  EXPECT_EQ(vec(0), 1);
  EXPECT_EQ(vec(1), 2);
  EXPECT_EQ(vec(2), 3);
  EXPECT_EQ(vec(3), 4);
}

////////////////////////////////////////////////////////////////////////////////
// Comparison.

TEST(Vector, Comparison) {
  EXPECT_TRUE(Vec4i(1, 2, 3, 4) == Vec4i(1, 2, 3, 4));
  EXPECT_FALSE(Vec4i(1, 2, 3, 4) == Vec4i(4, 3, 2, 1));

  EXPECT_FALSE(Vec4i(1, 2, 3, 4) != Vec4i(1, 2, 3, 4));
  EXPECT_TRUE(Vec4i(1, 2, 3, 4) != Vec4i(4, 3, 2, 1));
}

////////////////////////////////////////////////////////////////////////////////
// Mathematical operations.

TEST(Vector, Negate) { EXPECT_EQ(-Vec3i(1, -2, 3), Vec3i(-1, 2, -3)); }

TEST(Vector, UnaryAdd) {
  {
    Vec3i a(1, 2, 3);
    a += Vec3i(4, 5, 6);
    EXPECT_EQ(a, Vec3i(5, 7, 9));
  }
}

TEST(Vector, UnarySubtract) {
  {
    Vec3i a(5, 7, 9);
    a -= Vec3i(4, 5, 6);
    EXPECT_EQ(a, Vec3i(1, 2, 3));
  }
}

TEST(Vector, UnaryMultiplyByScalar) {
  {
    Vec3i a(1, 2, 3);
    a *= 2;
    EXPECT_EQ(a, Vec3i(2, 4, 6));
  }

  {
    Vec3 a(1, 2, 3);
    a *= DoubleDouble(2);
    EXPECT_THAT(a, Pointwise(DoubleNear(1e-12), Vec3(2, 4, 6)));
  }
}

TEST(Vector, UnaryDivideByScalar) {
  {
    Vec3i a(8, 10, 12);
    a /= 2;
    EXPECT_EQ(a, Vec3i(4, 5, 6));
  }

  {
    Vec3 a(8, 10, 12);
    a /= DoubleDouble(2);
    EXPECT_THAT(a, Pointwise(DoubleNear(1e-12), Vec3(4, 5, 6)));
  }
}

TEST(Vector, BinaryAdd) {
  EXPECT_EQ(Vec3i(1, 2, 3) + Vec3i(4, 5, 6), Vec3i(5, 7, 9));
}

TEST(Vector, BinarySubtract) {
  EXPECT_EQ(Vec3i(5, 7, 9) - Vec3i(4, 5, 6), Vec3i(1, 2, 3));
}

TEST(Vector, BinaryMultiplyByScalar) {
  EXPECT_EQ(Vec3i(1, 2, 3) * 2, Vec3i(2, 4, 6));
  EXPECT_EQ(2 * Vec3i(1, 2, 3), Vec3i(2, 4, 6));

  EXPECT_THAT(Vec3(1, 2, 3) * DoubleDouble(2),
              Pointwise(DoubleNear(1e-12), Vec3(2, 4, 6)));
  EXPECT_THAT(DoubleDouble(2) * Vec3(1, 2, 3),
              Pointwise(DoubleNear(1e-12), Vec3(2, 4, 6)));
}

TEST(Vector, BinaryDivideByScalar) {
  EXPECT_EQ(Vec3i(8, 10, 12) / 2, Vec3i(4, 5, 6));

  EXPECT_THAT(Vec3(8, 10, 12) / DoubleDouble(2),
              Pointwise(DoubleNear(1e-12), Vec3(4, 5, 6)));
}

////////////////////////////////////////////////////////////////////////////////
// Algebraic operations.

TEST(Vector, SquaredNorm) { EXPECT_EQ(Vec3i(1, -2, 3).SquaredNorm(), 14); }

TEST(Vector, Norm) { EXPECT_NEAR(Vec3(1, -2, 3).Norm(), Sqrt(14.0), 1e-12); }

TEST(Vector, Cross) {
  EXPECT_EQ(Vec3i(1, 2, 3).Cross(Vec3i(4, 5, 6)), Vec3i(-3, 6, -3));
  EXPECT_EQ(Vec2i(1, 2).Cross(Vec3i(4, 5, 6)), Vec3i(12, -6, -3));
  EXPECT_EQ(Vec3i(1, 2, 3).Cross(Vec2i(4, 5)), Vec3i(-15, 12, -3));
}

TEST(Vector, Dot) { EXPECT_EQ(Vec3i(1, 2, 3).Dot(Vec3i(4, 5, 6)), 32); }

TEST(Vector, Normalized) {
  EXPECT_THAT(Vec3(2, 0, 0).Normalized(),
              Pointwise(DoubleNear(1e-12), Vec3(1, 0, 0)));
  EXPECT_THAT(Vec3(0, 0.5, 0).Normalized(),
              Pointwise(DoubleNear(1e-12), Vec3(0, 1, 0)));
  EXPECT_THAT(Vec3(0, 0, -0.1).Normalized(),
              Pointwise(DoubleNear(1e-12), Vec3(0, 0, -1)));
}

TEST(Vector, CwiseAbs) {
  EXPECT_THAT(Vec3(-1, 0, 3).CwiseAbs(),
              Pointwise(DoubleNear(1e-12), Vec3(1, 0, 3)));
}

TEST(Vector, CwiseMin) {
  EXPECT_THAT(Vec3(-1, 0, 3).CwiseMin(Vec3(-2, 1, 3)),
              Pointwise(DoubleNear(1e-12), Vec3(-2, 0, 3)));
}

TEST(Vector, CwiseMax) {
  EXPECT_THAT(Vec3(-1, 0, 3).CwiseMax(Vec3(-2, 1, 3)),
              Pointwise(DoubleNear(1e-12), Vec3(-1, 1, 3)));
}

////////////////////////////////////////////////////////////////////////////////
// Output.

TEST(Vector, Print) {
  std::stringstream stream;
  stream << Vec3i{1, 2, 3};
  EXPECT_EQ(stream.str(), "1 2 3");
}

////////////////////////////////////////////////////////////////////////////////
// Integration with the Google testing framework.

TEST(Vector, PointwiseMatcher) {
  EXPECT_THAT(Vec3i(1, -2, 3), Pointwise(Eq(), {1, -2, 3}));
}

}  // namespace numeric_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
