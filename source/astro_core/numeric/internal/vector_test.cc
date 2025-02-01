// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/numeric/internal/vector.h"

#include <complex>
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

using Complex = std::complex<double>;
using ComplexI = std::complex<int>;

using Vec2i = Vector<int, 2>;
using Vec3i = Vector<int, 3>;
using Vec4i = Vector<int, 4>;

using Vec2 = Vector<double, 2>;
using Vec3 = Vector<double, 3>;
using Vec4 = Vector<double, 4>;

using Vec3c = Vector<Complex, 3>;
using Vec3ci = Vector<ComplexI, 3>;

////////////////////////////////////////////////////////////////////////////////
// Constructor.

TEST(Vector, Construct) {
  // Test initialization of zero vector.
  {
    EXPECT_THAT(Vec4i::Zero().Data(), Pointwise(Eq(), {0, 0, 0, 0}));
  }

  // Test initialization of vector with all elements set to the given value.
  {
    EXPECT_THAT(Vec4i::Constant(3).Data(), Pointwise(Eq(), {3, 3, 3, 3}));
  }

  // Test vector construction from the given elements.
  {
    EXPECT_THAT(Vec4i(1, 2, 3, 4).Data(), Pointwise(Eq(), {1, 2, 3, 4}));
  }

  // Test copy constructor.
  {
    Vec3i a{1, 2, 3};
    Vec3i b{a};
    EXPECT_THAT(b.Data(), Pointwise(Eq(), {1, 2, 3}));
  }

  // Test copy constructor with type cast.
  {
    // Double from integer.
    {
      Vec3i a{1, 2, 3};
      Vec3 b{a};
      EXPECT_THAT(b.Data(), Pointwise(DoubleNear(1e-12), {1, 2, 3}));
    }

    // Integer from double.
    {
      Vec3 a{1, 2, 3};
      Vec3i b{a};
      EXPECT_THAT(b.Data(), Pointwise(Eq(), {1, 2, 3}));
    }

    // Complex from integer.
    {
      Vec3 a{1, 2, 3};
      Vec3ci b{a};
      EXPECT_THAT(b.Data(),
                  Pointwise(Eq(), {ComplexI(1), ComplexI(2), ComplexI(3)}));
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Type conversion.

TEST(Vector, Cast) {
  // Integer to double.
  {
    const Vec3i a{1, 2, 3};
    Vec3 b = a.Cast<double>();
    EXPECT_THAT(b.Data(), Pointwise(DoubleNear(1e-12), {1, 2, 3}));
  }

  // Double to integer.
  {
    const Vec3 a{1, 2, 3};
    Vec3i b = a.Cast<int>();
    EXPECT_THAT(b.Data(), Pointwise(Eq(), {1, 2, 3}));
  }

  // Integer to complex.
  {
    const Vec3 a{1, 2, 3};
    Vec3ci b = a.Cast<ComplexI>();
    EXPECT_THAT(b.Data(),
                Pointwise(Eq(), {ComplexI(1), ComplexI(2), ComplexI(3)}));
  }
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

  {
    Vec3ci a(ComplexI(1, 2), ComplexI(3, 4), ComplexI(5, 6));
    a *= 2;
    EXPECT_EQ(a, Vec3ci(ComplexI(2, 4), ComplexI(6, 8), ComplexI(10, 12)));
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

  {
    Vec3ci a(ComplexI(2, 4), ComplexI(6, 8), ComplexI(10, 12));
    a /= 2;
    EXPECT_EQ(a, Vec3ci(ComplexI(1, 2), ComplexI(3, 4), ComplexI(5, 6)));
  }
}

TEST(Vector, BinaryAdd) {
  // Vectors of the same type.
  {
    EXPECT_EQ(Vec3i(1, 2, 3) + Vec3i(4, 5, 6), Vec3i(5, 7, 9));
  }

  // Vector of elements with different types.
  {
    const Vec3ci a(ComplexI(2, 4), ComplexI(6, 8), ComplexI(10, 12));
    const Vec3i b(4, 5, 6);

    const Vec3ci c = a + b;
    EXPECT_EQ(c, Vec3ci(ComplexI(6, 4), ComplexI(11, 8), ComplexI(16, 12)));

    const Vec3ci d = b + a;
    EXPECT_EQ(d, Vec3ci(ComplexI(6, 4), ComplexI(11, 8), ComplexI(16, 12)));
  }
}

TEST(Vector, BinarySubtract) {
  // Vectors of the same type.
  {
    EXPECT_EQ(Vec3i(5, 7, 9) - Vec3i(4, 5, 6), Vec3i(1, 2, 3));
  }

  // Vector of elements with different types.
  {
    const Vec3ci a(ComplexI(2, 4), ComplexI(6, 8), ComplexI(10, 12));
    const Vec3i b(4, 5, 6);

    const Vec3ci c = a - b;
    EXPECT_EQ(c, Vec3ci(ComplexI(-2, 4), ComplexI(1, 8), ComplexI(4, 12)));

    const Vec3ci d = b - a;
    EXPECT_EQ(d, Vec3ci(ComplexI(2, -4), ComplexI(-1, -8), ComplexI(-4, -12)));
  }
}

TEST(Vector, BinaryMultiplyByScalar) {
  // Multiplication between integer vector and integer scalar.
  {
    EXPECT_EQ(Vec3i(1, 2, 3) * 2, Vec3i(2, 4, 6));
    EXPECT_EQ(2 * Vec3i(1, 2, 3), Vec3i(2, 4, 6));
  }

  // Multiplication between vector and scalar of different types.
  {
    const Vec3ci a = Vec3ci(ComplexI(1, 2), ComplexI(3, 4), ComplexI(5, 6)) * 2;
    EXPECT_EQ(a, Vec3ci(ComplexI(2, 4), ComplexI(6, 8), ComplexI(10, 12)));

    const Vec3ci b = Vec3i(1, 2, 3) * ComplexI(2, 3);
    EXPECT_EQ(b, Vec3ci(ComplexI(2, 3), ComplexI(4, 6), ComplexI(6, 9)));
  }

  // Multiplication between vector and double-double scalar.
  // Similar to above, just to ensure specific use-case used in the other parts
  // of the project.
  {
    const Vector<DoubleDouble, 3> a = Vec3(1, 2, 3) * DoubleDouble(2);
    EXPECT_THAT(a.Cast<double>(), Pointwise(DoubleNear(1e-12), Vec3(2, 4, 6)));

    const Vector<DoubleDouble, 3> b = DoubleDouble(2) * Vec3(1, 2, 3);
    EXPECT_THAT(b.Cast<double>(), Pointwise(DoubleNear(1e-12), Vec3(2, 4, 6)));
  }
}

TEST(Vector, BinaryDivideByScalar) {
  // Division of integer vector by an integer scalar.
  {
    EXPECT_EQ(Vec3i(8, 10, 12) / 2, Vec3i(4, 5, 6));
  }

  // Division on vector and scalar of different types.
  {
    const Vec3ci a =
        Vec3ci(ComplexI(2, 4), ComplexI(6, 8), ComplexI(10, 12)) / 2;
    EXPECT_EQ(a, Vec3ci(ComplexI(1, 2), ComplexI(3, 4), ComplexI(5, 6)));

    const Vec3c b = Vec3(1, 2, 3) / Complex(4, 5);
    EXPECT_NEAR(b(0).real(), 0.09756097560975611, 1e-12);
    EXPECT_NEAR(b(0).imag(), -0.12195121951219513, 1e-12);
    EXPECT_NEAR(b(1).real(), 0.19512195121951223, 1e-12);
    EXPECT_NEAR(b(1).imag(), -0.24390243902439027, 1e-12);
    EXPECT_NEAR(b(2).real(), 0.2926829268292684, 1e-12);
    EXPECT_NEAR(b(2).imag(), -0.3658536585365854, 1e-12);
  }

  // Division of vector by a double-double scalar.
  // Similar to above, just to ensure specific use-case used in the other parts
  // of the project.
  {
    const Vector<DoubleDouble, 3> a = Vec3(8, 10, 12) / DoubleDouble(2);
    EXPECT_THAT(a.Cast<double>(), Pointwise(DoubleNear(1e-12), Vec3(4, 5, 6)));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Algebraic operations.

TEST(Vector, SquaredNorm) { EXPECT_EQ(Vec3i(1, -2, 3).SquaredNorm(), 14); }

TEST(Vector, Norm) { EXPECT_NEAR(Vec3(1, -2, 3).Norm(), Sqrt(14.0), 1e-12); }

TEST(Vector, Cross) {
  // Vectors of the same type.
  {
    EXPECT_EQ(Vec3i(1, 2, 3).Cross(Vec3i(4, 5, 6)), Vec3i(-3, 6, -3));
    EXPECT_EQ(Vec2i(1, 2).Cross(Vec3i(4, 5, 6)), Vec3i(12, -6, -3));
    EXPECT_EQ(Vec3i(1, 2, 3).Cross(Vec2i(4, 5)), Vec3i(-15, 12, -3));
  }

  // Vectors with elements of different types.
  {
    EXPECT_THAT(Vec3i(1, 2, 3).Cross(Vec3(4, 5, 6)),
                Pointwise(DoubleNear(1e-12), Vec3i(-3, 6, -3)));
    EXPECT_THAT(Vec2i(1, 2).Cross(Vec3(4, 5, 6)),
                Pointwise(DoubleNear(1e-12), Vec3i(12, -6, -3)));
    EXPECT_THAT(Vec3i(1, 2, 3).Cross(Vec2(4, 5)),
                Pointwise(DoubleNear(1e-12), Vec3i(-15, 12, -3)));
  }
}

TEST(Vector, Dot) {
  // Vectors of the same type.
  {
    EXPECT_EQ(Vec3i(1, 2, 3).Dot(Vec3i(4, 5, 6)), 32);
  }

  // Vectors with elements of different types.
  {
    const Vec3i a = Vec3i(1, 2, 3);
    const Vec3 b = Vec3(4, 5, 6);
    EXPECT_NEAR(a.Dot(b), 32.0, 1e-12);
  }
}

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
