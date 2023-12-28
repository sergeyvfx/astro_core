// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/tle.h"

#include "astro_core/satellite/tle_parser.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

namespace {

// Telementry of the ISS (ZARYA) from Wikipedia.
struct ISS {
  static constexpr const char* kLine1 =
      "1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927";
  static constexpr const char* kLine2 =
      "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537";
};

// An analyst object seen by the space fence.
// https://github.com/brandon-rhodes/python-sgp4/issues/77
struct T00000 {
  static constexpr const char* kLine1 =
      "1 T0000U          20341.14572529  .00000446  00000-0  15605-2 0  9998";
  static constexpr const char* kLine2 =
      "2 T0000  90.2902 300.0888 0031941  22.1325 338.1165 12.95152933 48676";
};

template <class TLEStrings>
auto CreateTLE() -> TLE {
  const TLEParser::Result result =
      TLEParser::FromLines(TLEStrings::kLine1, TLEStrings::kLine2);

  EXPECT_TRUE(result.Ok());

  return result.GetValue();
}

}  // namespace

TEST(TLE, CalculateTLELineChecksum) {
  EXPECT_EQ(CalculateTLELineChecksum(ISS::kLine1) % 10, 7);
}

TEST(TLE, FirstLineAsString) {
  {
    const TLE tle = CreateTLE<ISS>();
    EXPECT_EQ(tle.FirstLineAsString(), ISS::kLine1);
  }

  {
    const TLE tle = CreateTLE<T00000>();
    EXPECT_EQ(tle.FirstLineAsString(), T00000::kLine1);
  }
}

TEST(TLE, SecondLineAsString) {
  {
    const TLE tle = CreateTLE<ISS>();
    EXPECT_EQ(tle.SecondLineAsString(), ISS::kLine2);
  }

  {
    const TLE tle = CreateTLE<T00000>();
    EXPECT_EQ(tle.SecondLineAsString(), T00000::kLine2);
  }
}

}  // namespace astro_core
