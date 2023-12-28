// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/parse/field_parser.h"

#include "astro_core/parse/arithmetic.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

namespace {

enum class Error { KError };
using Result = astro_core::Result<int, Error>;

auto StringToIntValueParser(const std::string_view str) -> Result {
  return Result(astro_core::StringToInt<int>(str));
}

}  // namespace

TEST(FieldParser, GetFieldView) {
  EXPECT_EQ(GetFieldView("Hello", 80, 6), "");
  EXPECT_EQ(GetFieldView("Hello, World!", 6, 6), ",");
  EXPECT_EQ(GetFieldView("Hello, World!", 1, 5), "Hello");
  EXPECT_EQ(GetFieldView("Hello, World!", 8, 6), "World!");
  EXPECT_EQ(GetFieldView("Hello", 1, 50), "Hello");
}

TEST(FieldParser, ParseField) {
  struct Object {
    int field;
  };

  // Field starts from the beginning of the line.
  {
    Object object{-1};
    const bool result =
        ParseField<Object, &Object::field, IntReader<int>::Read, 1, 5>(
            object, "12345 678 9");
    EXPECT_TRUE(result);
    EXPECT_EQ(object.field, 12345);
  }

  // Field is in the middle of the string.
  {
    Object object{-1};
    const bool result =
        ParseField<Object, &Object::field, IntReader<int>::Read, 7, 9>(
            object, "12345 678 9");
    EXPECT_TRUE(result);
    EXPECT_EQ(object.field, 678);
  }

  // Field which can not be fully interpreted as integer.
  {
    Object object{-1};
    const bool result =
        ParseField<Object, &Object::field, IntReader<int>::Read, 1, 5>(
            object, "1ello 678 9");
    EXPECT_TRUE(result);
    EXPECT_EQ(object.field, 1);
  }
  {
    Object object{-1};
    const bool result =
        ParseField<Object, &Object::field, IntReader<int>::Convert, 1, 5>(
            object, "1ello 678 9");
    EXPECT_FALSE(result);
    EXPECT_EQ(object.field, -1);
  }

  // Use parser function.
  {
    Object object{-1};
    const bool result =
        ParseField<Object, &Object::field, StringToIntValueParser, 7, 9>(
            object, "12345 678 9");
    EXPECT_TRUE(result);
    EXPECT_EQ(object.field, 678);
  }
}

TEST(FieldParser, RunFieldParsers) {
  struct Object {
    int field_a;
    int field_b;
  };

  {
    Object object{-1, -1};
    const bool result = RunFieldParsers(
        object,
        "12345 678 9",
        ParseField<Object, &Object::field_a, IntReader<int>::Convert, 1, 5>,
        ParseField<Object, &Object::field_b, IntReader<int>::Convert, 7, 9>);
    EXPECT_TRUE(result);
    EXPECT_EQ(object.field_a, 12345);
    EXPECT_EQ(object.field_b, 678);
  }
}

TEST(FieldParser, TryParseField) {
  struct Object {
    int field;
  };

  {
    Object object{-1};
    const bool result =
        TryParseField<Object,
                      &Object::field,
                      IntReader<int>::Convert,
                      FieldColumns{1, 3},
                      FieldColumns{7, 9}>(object, "1ello 678 9");
    EXPECT_TRUE(result);
    EXPECT_EQ(object.field, 678);
  }
}

}  // namespace astro_core
