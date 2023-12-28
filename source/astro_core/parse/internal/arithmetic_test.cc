// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/parse/arithmetic.h"

#include <string_view>

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(IntReader, Read) {
  {
    std::string_view str("");
    IntReader<int>::Result result = IntReader<int>::Read(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("  123");
    IntReader<int>::Result result = IntReader<int>::Read(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123);
    EXPECT_EQ(str, "");
  }

  {
    std::string_view str("123 456");
    IntReader<int>::Result result = IntReader<int>::Read(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123);
    EXPECT_EQ(str, " 456");
  }
}

TEST(IntReader, Convert) {
  {
    std::string_view str("");
    IntReader<int>::Result result = IntReader<int>::Convert(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("123x");
    IntReader<int>::Result result = IntReader<int>::Convert(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("123");
    IntReader<int>::Result result = IntReader<int>::Convert(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123);
  }

  {
    std::string_view str("123 ");
    IntReader<int>::Result result = IntReader<int>::Convert(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123);
  }
}

TEST(FloatReader, Read) {
  {
    std::string_view str("");
    FloatReader<float>::Result result = FloatReader<float>::Read(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("  123");
    FloatReader<float>::Result result = FloatReader<float>::Read(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123);
    EXPECT_EQ(str, "");
  }

  {
    std::string_view str("123.4 567");
    FloatReader<float>::Result result = FloatReader<float>::Read(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123.4f);
    EXPECT_EQ(str, " 567");
  }
}

TEST(FloatReader, Convert) {
  {
    std::string_view str("");
    FloatReader<float>::Result result = FloatReader<float>::Convert(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("123x");
    FloatReader<float>::Result result = FloatReader<float>::Convert(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("123.4.5");
    FloatReader<float>::Result result = FloatReader<float>::Convert(str);
    EXPECT_FALSE(result.Ok());
  }

  {
    std::string_view str("123.4");
    FloatReader<float>::Result result = FloatReader<float>::Convert(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123.4f);
  }

  {
    std::string_view str("123.4 ");
    FloatReader<float>::Result result = FloatReader<float>::Convert(str);
    EXPECT_TRUE(result.Ok());
    EXPECT_EQ(result.GetValue(), 123.4f);
  }
}

}  // namespace astro_core
