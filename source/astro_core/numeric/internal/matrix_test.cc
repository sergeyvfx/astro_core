// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/numeric/internal/matrix.h"

#include <sstream>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace numeric_internal {

using testing::Eq;
using testing::Pointwise;

using Vec2i = Vector<int, 2>;
using Vec3i = Vector<int, 3>;

using Mat2x3i = Matrix<int, 2, 3>;

using Mat3i = Matrix<int, 3, 3>;
using Mat3x2i = Matrix<int, 3, 2>;
using Mat3x4i = Matrix<int, 3, 4>;

using Mat5x3i = Matrix<int, 5, 3>;
using Mat5x4i = Matrix<int, 5, 4>;

////////////////////////////////////////////////////////////////////////////////
// Constructor.

TEST(Matrix, Construct) {
  EXPECT_THAT(Mat2x3i::Zero().Data(), Pointwise(Eq(), {0, 0, 0, 0, 0, 0}));

  EXPECT_THAT(Mat2x3i::Identity().Data(), Pointwise(Eq(), {1, 0, 0, 1, 0, 0}));

  EXPECT_THAT(Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}).Data(),
              Pointwise(Eq(), {1, 2, 3, 4, 5, 6}));

  EXPECT_THAT(Mat2x3i::FromRows({{1, 2, 3}, {4, 5, 6}}).Data(),
              Pointwise(Eq(), {1, 4, 2, 5, 3, 6}));
}

////////////////////////////////////////////////////////////////////////////////
// Shape.

TEST(Matrix, Dimension) {
  EXPECT_EQ(Mat3i::kStaticNumRows, 3);
  EXPECT_EQ(Mat3i::kStaticNumColumns, 3);
  EXPECT_EQ(Mat3i().GetNumRows(), 3);
  EXPECT_EQ(Mat3i().GetNumColumns(), 3);

  EXPECT_EQ(Mat3x4i::kStaticNumRows, 3);
  EXPECT_EQ(Mat3x4i::kStaticNumColumns, 4);
  EXPECT_EQ(Mat3x4i().GetNumRows(), 3);
  EXPECT_EQ(Mat3x4i().GetNumColumns(), 4);
}

////////////////////////////////////////////////////////////////////////////////
// Comparison.

TEST(Matrix, Comparison) {
  EXPECT_TRUE(Mat3i::Identity() == Mat3i::Identity());
  EXPECT_FALSE(Mat3i::Identity() == Mat3i::Zero());

  EXPECT_FALSE(Mat3i::Identity() != Mat3i::Identity());
  EXPECT_TRUE(Mat3i::Identity() != Mat3i::Zero());
}

////////////////////////////////////////////////////////////////////////////////
// Mathematical operations.

TEST(Matrix, Negate) {
  (-Mat2x3i::FromColumns({{1, -2}, {-3, 4}, {-5, -6}})).Data(),
      Pointwise(Eq(), {-1, 2, 3, -4, 5, 6});
}

TEST(Matrix, UnaryAdd) {
  {
    Mat2x3i a = Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}});
    a += Mat2x3i::FromColumns({{7, 8}, {9, 10}, {11, 12}});
    EXPECT_EQ(a, Mat2x3i::FromColumns({{8, 10}, {12, 14}, {16, 18}}));
  }
}

TEST(Matrix, UnarySubtract) {
  {
    Mat2x3i a = Mat2x3i::FromColumns({{8, 10}, {12, 14}, {16, 18}});
    a -= Mat2x3i::FromColumns({{7, 8}, {9, 10}, {11, 12}});
    EXPECT_EQ(a, Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}));
  }
}

TEST(Matrix, UnaryMultiplyByScalar) {
  {
    Mat2x3i a = Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}});
    a *= 2;
    EXPECT_EQ(a, Mat2x3i::FromColumns({{2, 4}, {6, 8}, {10, 12}}));
  }
}

TEST(Matrix, UnaryDivideByScalar) {
  {
    Mat2x3i a = Mat2x3i::FromColumns({{2, 4}, {6, 8}, {10, 12}});
    a /= 2;
    EXPECT_EQ(a, Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}));
  }
}

TEST(Matrix, BinaryAdd) {
  EXPECT_EQ(Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}) +
                Mat2x3i::FromColumns({{7, 8}, {9, 10}, {11, 12}}),
            Mat2x3i::FromColumns({{8, 10}, {12, 14}, {16, 18}}));
}

TEST(Matrix, BinarySubtract) {
  EXPECT_EQ(Mat2x3i::FromColumns({{8, 10}, {12, 14}, {16, 18}}) -
                Mat2x3i::FromColumns({{7, 8}, {9, 10}, {11, 12}}),
            Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}));
}

TEST(Matrix, BinaryMultiplyByScalar) {
  EXPECT_EQ(Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}) * 2,
            Mat2x3i::FromColumns({{2, 4}, {6, 8}, {10, 12}}));

  EXPECT_EQ(2 * Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}),
            Mat2x3i::FromColumns({{2, 4}, {6, 8}, {10, 12}}));
}

TEST(Matrix, BinaryMultiplyByMatrix) {
  {
    EXPECT_EQ(Mat5x3i::FromRows({
                  {1, 2, 3},
                  {4, 5, 6},
                  {7, 8, 9},
                  {10, 11, 12},
                  {13, 14, 15},
              }) * Mat3x4i::FromRows({
                       {1, 2, 3, 4},
                       {5, 6, 7, 8},
                       {9, 10, 11, 12},
                   }),
              Mat5x4i::FromRows({{38, 44, 50, 56},
                                 {83, 98, 113, 128},
                                 {128, 152, 176, 200},
                                 {173, 206, 239, 272},
                                 {218, 260, 302, 344}}));
  }
}

TEST(Matrix, BinaryDivideByScalar) {
  EXPECT_EQ(Mat2x3i::FromColumns({{2, 4}, {6, 8}, {10, 12}}) / 2,
            Mat2x3i::FromColumns({{1, 2}, {3, 4}, {5, 6}}));
}

TEST(Matrix, RightMatrixByVectorMultiplication) {
  EXPECT_EQ(Mat2x3i::FromRows({
                {1, 2, 3},
                {4, 5, 6},
            }) * Vec3i({1, 2, 3}),
            Vec2i({14, 32}));
}

////////////////////////////////////////////////////////////////////////////////
// Arithmetic.

TEST(Matrix, Transposed) {
  EXPECT_EQ(Mat2x3i::FromRows({{1, 2, 3}, {4, 5, 6}}).Transposed(),
            Mat3x2i::FromColumns({{1, 2, 3}, {4, 5, 6}}));

  EXPECT_EQ(Mat3i::FromRows({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}).Transposed(),
            Mat3i::FromColumns({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}));
}

////////////////////////////////////////////////////////////////////////////////
// Output.

TEST(Matrix, Print) {
  std::stringstream stream;
  stream << Mat2x3i::Identity();
  EXPECT_EQ(stream.str(), "1 0 0\n0 1 0");
}

////////////////////////////////////////////////////////////////////////////////
// Integration with the Google testing framework.

TEST(Matrix, PointwiseMatcher) {
  EXPECT_THAT(Mat2x3i::Identity(), Pointwise(Eq(), {1, 0, 0, 1, 0, 0}));
}

}  // namespace numeric_internal

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
