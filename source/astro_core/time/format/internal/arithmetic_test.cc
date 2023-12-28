// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/format/arithmetic.h"

#include <type_traits>

#include "astro_core/base/double_double.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::NearUsingAbsDifferenceMetric;

namespace {

namespace {

// Helper code generator for traits to detect whether there is a valid operator
// which is valid for `declval<T>() op declval<U>()`.
//
// For example:
//
//   DEFINE_HAS_OPERATOR_TRAITS(HasOperatorEqual, ==)
//
//   std::cout << (HasOperatorEqual<TypeA, TypeB>::value) ?
//                "Operation is defined" : "Operation is NOT defined";
#define DEFINE_HAS_OPERATOR_TRAITS(name, op)                                   \
  template <class T, class U, class = void>                                    \
  struct name : std::false_type {};                                            \
                                                                               \
  template <class T, class U>                                                  \
  struct name<T,                                                               \
              U,                                                               \
              std::void_t<decltype(std::declval<T>() op std::declval<U>())>>   \
      : std::true_type {};                                                     \
                                                                               \
  template <class T, class U>                                                  \
  inline constexpr bool name##V = name<T, U>::value;

DEFINE_HAS_OPERATOR_TRAITS(HasOperatorEqual, ==)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorLess, <)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorGreater, >)

DEFINE_HAS_OPERATOR_TRAITS(HasOperatorUnaryPlus, +=)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorUnaryMinus, -=)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorUnaryMultiply, *=)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorUnaryDivide, /=)

DEFINE_HAS_OPERATOR_TRAITS(HasOperatorBinaryPlus, +)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorBinaryMinus, -)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorBinaryMultiply, *)
DEFINE_HAS_OPERATOR_TRAITS(HasOperatorBinaryDivide, /)

#undef DEFINE_HAS_OPERATOR_TRAITS

}  // namespace

class MyFormat : public ArithmeticFormat<DoubleDouble, MyFormat> {
  using BaseClass = ArithmeticFormat<DoubleDouble, MyFormat>;

 public:
  using BaseClass::BaseClass;

  using BaseClass::operator<=>;
  using BaseClass::operator==;
};

class MyOtherFormat : public ArithmeticFormat<DoubleDouble, MyOtherFormat> {
  using BaseClass = ArithmeticFormat<DoubleDouble, MyOtherFormat>;

 public:
  using BaseClass::BaseClass;

  using BaseClass::operator<=>;
  using BaseClass::operator==;
};

}  // namespace

////////////////////////////////////////////////////////////////////////////////
// Core test.
//
// Make sure that the API needed for the gtest matcher is implemented and passes
// testing.

TEST(ArithmeticFormat, NearUsingAbsDifferenceMetric) {
  using testing::Not;

  EXPECT_THAT(MyFormat(1234.56),
              NearUsingAbsDifferenceMetric(MyFormat(1234.56), 1e-12));

  EXPECT_THAT(MyFormat(1234.5678),
              NearUsingAbsDifferenceMetric(MyFormat(1234.5699), 1e-2));

  EXPECT_THAT(MyFormat(1234.5678),
              Not(NearUsingAbsDifferenceMetric(MyFormat(1234.5699), 1e-12)));
}

////////////////////////////////////////////////////////////////////////////////
// Constructors and assignment.

TEST(ArithmeticFormat, Construct) {
  static_assert(!std::is_constructible_v<MyFormat, MyOtherFormat>);

  {
    const MyFormat format(1234.56);
    EXPECT_THAT(DoubleDouble(format),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }

  {
    const MyFormat format(DoubleDouble(1234.56));
    EXPECT_THAT(DoubleDouble(format),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }

  {
    const MyFormat existing_format(1234.56);
    const MyFormat format(existing_format);
    EXPECT_THAT(DoubleDouble(format),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }
}

TEST(ArithmeticFormat, Assign) {
  static_assert(!std::is_assignable_v<MyFormat, MyOtherFormat>);

  {
    MyFormat format;
    format = MyFormat(1234.56);
    EXPECT_THAT(DoubleDouble(format),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }

  {
    MyFormat format;
    format = DoubleDouble(1234.56);
    EXPECT_THAT(DoubleDouble(format),
                NearUsingAbsDifferenceMetric(DoubleDouble(1234.56), 1e-12));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Negation.

TEST(ArithmeticFormat, Negate) {
  EXPECT_EQ(-MyFormat(0), MyFormat(0));
  EXPECT_EQ(-MyFormat(1024.2048), MyFormat(-1024.2048));
  EXPECT_EQ(-MyFormat(4096.8192), MyFormat(-4096.8192));
}

////////////////////////////////////////////////////////////////////////////////
// Unary operations.

TEST(ArithmeticFormat, UnaryAdd) {
  static_assert(HasOperatorUnaryPlusV<MyFormat, MyFormat>);
  static_assert(!HasOperatorUnaryPlusV<MyFormat, MyOtherFormat>);

  // ArithmeticFormat += double
  {
    MyFormat a(1024.2048);
    a += 4096.8192;
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }

  // ArithmeticFormat += DoubleDouble
  {
    MyFormat a(1024.2048);
    a += DoubleDouble(4096.8192);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }

  // ArithmeticFormat += ArithmeticFormat
  {
    MyFormat a(1024.2048);
    a += MyFormat(4096.8192);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }
}

TEST(ArithmeticFormat, UnarySubtract) {
  static_assert(HasOperatorUnaryMinusV<MyFormat, MyFormat>);
  static_assert(!HasOperatorUnaryMinusV<MyFormat, MyOtherFormat>);

  // DoubleDouble -= double
  {
    MyFormat a(1024.2048);
    a -= 4096.8192;
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }

  // ArithmeticFormat -= DoubleDouble
  {
    MyFormat a(1024.2048);
    a -= DoubleDouble(4096.8192);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }

  // ArithmeticFormat -= MyFormat
  {
    MyFormat a(1024.2048);
    a -= MyFormat(4096.8192);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }
}

TEST(ArithmeticFormat, UnaryMultiply) {
  static_assert(HasOperatorUnaryMultiplyV<MyFormat, MyFormat>);
  static_assert(!HasOperatorUnaryMultiplyV<MyFormat, MyOtherFormat>);

  // ArithmeticFormat *= double
  {
    MyFormat a(1024.2048);
    a *= 10.25;
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }

  // ArithmeticFormat *= DoubleDouble
  {
    MyFormat a(1024.2048);
    a *= DoubleDouble(10.25);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }

  // ArithmeticFormat *= ArithmeticFormat
  {
    MyFormat a(1024.2048);
    a *= MyFormat(10.25);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }
}

TEST(ArithmeticFormat, UnaryDivide) {
  static_assert(HasOperatorUnaryDivideV<MyFormat, MyFormat>);
  static_assert(!HasOperatorUnaryDivideV<MyFormat, MyOtherFormat>);

  // ArithmeticFormat *= double
  {
    MyFormat a(10498.0992);
    a /= 10.25;
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }

  // ArithmeticFormat *= DoubleDouble
  {
    MyFormat a(10498.0992);
    a /= DoubleDouble(10.25);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }

  // ArithmeticFormat *= ArithmeticFormat
  {
    MyFormat a(10498.0992);
    a /= MyFormat(10.25);
    EXPECT_THAT(a, NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Binary operations.

TEST(ArithmeticFormat, BinaryAdd) {
  static_assert(HasOperatorBinaryPlusV<MyFormat, MyFormat>);
  static_assert(!HasOperatorBinaryPlusV<MyFormat, MyOtherFormat>);

  // ArithmeticFormat + double
  {
    EXPECT_THAT(MyFormat(1024.2048) + 4096.8192,
                NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }

  // double + ArithmeticFormat
  {
    EXPECT_THAT(4096.8192 + MyFormat(1024.2048),
                NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }

  // ArithmeticFormat + DoubleDouble
  {
    EXPECT_THAT(MyFormat(1024.2048) + DoubleDouble(4096.8192),
                NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }

  // DoubleDouble + ArithmeticFormat
  {
    EXPECT_THAT(DoubleDouble(4096.8192) + MyFormat(1024.2048),
                NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }

  // ArithmeticFormat + ArithmeticFormat
  {
    EXPECT_THAT(MyFormat(1024.2048) + MyFormat(4096.8192),
                NearUsingAbsDifferenceMetric(MyFormat(5121.024), 1e-12));
  }
}

TEST(ArithmeticFormat, BinarySubtract) {
  static_assert(HasOperatorBinaryMinusV<MyFormat, MyFormat>);
  static_assert(!HasOperatorBinaryMinusV<MyFormat, MyOtherFormat>);

  // ArithmeticFormat - DoubleDouble
  {
    EXPECT_THAT(MyFormat(1024.2048) - DoubleDouble(4096.8192),
                NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }

  // DoubleDouble - ArithmeticFormat
  {
    EXPECT_THAT(DoubleDouble(1024.2048) - MyFormat(4096.8192),
                NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }

  // ArithmeticFormat - double
  {
    EXPECT_THAT(MyFormat(1024.2048) - 4096.8192,
                NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }

  // double - ArithmeticFormat
  {
    EXPECT_THAT(1024.2048 - MyFormat(4096.8192),
                NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }

  // ArithmeticFormat - ArithmeticFormat
  {
    EXPECT_THAT(MyFormat(1024.2048) - MyFormat(4096.8192),
                NearUsingAbsDifferenceMetric(MyFormat(-3072.6144), 1e-12));
  }
}

TEST(ArithmeticFormat, BinaryMultiply) {
  static_assert(HasOperatorBinaryMultiplyV<MyFormat, MyFormat>);
  static_assert(!HasOperatorBinaryMultiplyV<MyFormat, MyOtherFormat>);

  // double * ArithmeticFormat
  {
    EXPECT_THAT(1024.2048 * MyFormat(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }

  // ArithmeticFormat * double
  {
    EXPECT_THAT(MyFormat(1024.2048) * 10.25,
                NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }

  // DoubleDouble * ArithmeticFormat
  {
    EXPECT_THAT(DoubleDouble(1024.2048) * MyFormat(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }

  // ArithmeticFormat * DoubleDouble
  {
    EXPECT_THAT(MyFormat(1024.2048) * DoubleDouble(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }

  // ArithmeticFormat * ArithmeticFormat
  {
    EXPECT_THAT(MyFormat(1024.2048) * MyFormat(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(10498.0992), 1e-12));
  }
}

TEST(ArithmeticFormat, BinaryDivide) {
  static_assert(HasOperatorBinaryDivideV<MyFormat, MyFormat>);
  static_assert(!HasOperatorBinaryDivideV<MyFormat, MyOtherFormat>);

  // double / ArithmeticFormat
  {
    EXPECT_THAT(10498.0992 / MyFormat(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }

  // ArithmeticFormat / double
  {
    EXPECT_THAT(MyFormat(10498.0992) / 10.25,
                NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }

  // DoubleDouble / ArithmeticFormat
  {
    EXPECT_THAT(DoubleDouble(10498.0992) / MyFormat(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }

  // ArithmeticFormat / DoubleDouble
  {
    EXPECT_THAT(MyFormat(10498.0992) / DoubleDouble(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }

  // ArithmeticFormat * ArithmeticFormat
  {
    EXPECT_THAT(MyFormat(10498.0992) / MyFormat(10.25),
                NearUsingAbsDifferenceMetric(MyFormat(1024.2048), 1e-12));
  }
}

////////////////////////////////////////////////////////////////////////////////
// Comparison.

TEST(ArithmeticFormat, Compare) {
  static_assert(HasOperatorEqualV<MyFormat, MyFormat>);
  static_assert(HasOperatorLessV<MyFormat, MyFormat>);
  static_assert(HasOperatorGreaterV<MyFormat, MyFormat>);

  static_assert(!HasOperatorEqualV<MyFormat, MyOtherFormat>);
  static_assert(!HasOperatorLessV<MyFormat, MyOtherFormat>);
  static_assert(!HasOperatorGreaterV<MyFormat, MyOtherFormat>);

  // Compare with MyFormat.
  {
    EXPECT_TRUE(MyFormat(100) == MyFormat(100));
    EXPECT_FALSE(MyFormat(100) == MyFormat(200));

    EXPECT_FALSE(MyFormat(100) != MyFormat(100));
    EXPECT_TRUE(MyFormat(100) != MyFormat(200));

    EXPECT_TRUE(MyFormat(100) < MyFormat(200));
    EXPECT_FALSE(MyFormat(200) < MyFormat(100));

    EXPECT_FALSE(MyFormat(100) > MyFormat(200));
    EXPECT_TRUE(MyFormat(200) > MyFormat(100));
  }

  // Compare with scalar value.
  {
    EXPECT_TRUE(MyFormat(100) == 100);
    EXPECT_FALSE(MyFormat(100) == 200);

    EXPECT_FALSE(MyFormat(100) != 100);
    EXPECT_TRUE(MyFormat(100) != 200);

    EXPECT_TRUE(MyFormat(100) < 200);
    EXPECT_FALSE(MyFormat(200) < 100);

    EXPECT_FALSE(MyFormat(100) > 200);
    EXPECT_TRUE(MyFormat(200) > 100);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Non-member functions.

TEST(ArithmeticFormat, Abs) {
  EXPECT_EQ(Abs(MyFormat(1024.2048)), MyFormat(1024.2048));
  EXPECT_EQ(Abs(MyFormat(-1024.2048)), MyFormat(1024.2048));
  EXPECT_EQ(Abs(-MyFormat(1024.2048)), MyFormat(1024.2048));
}

}  // namespace astro_core
