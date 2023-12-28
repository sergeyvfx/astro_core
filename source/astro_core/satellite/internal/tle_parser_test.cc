// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/tle_parser.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

// Telemetry of the ISS (ZARYA) from Wikipedia.
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

TEST(TLEParser, FromLines) {
  // Old-style TLE.
  {
    const TLEParser::Result result =
        TLEParser::FromLines(ISS::kLine1, ISS::kLine2);

    EXPECT_TRUE(result.Ok());

    const TLE& tle = result.GetValue();

    EXPECT_EQ(tle.satellite_catalog_number, 25544);
    EXPECT_EQ(tle.classification, TLE::Classification::kUnclassified);

    EXPECT_EQ(tle.international_designator.GetYear(), 1998);
    EXPECT_EQ(tle.international_designator.GetNumber(), 67);
    EXPECT_EQ(tle.international_designator.GetPiece(), "A");

    EXPECT_EQ(tle.epoch.GetYear(), 2008);
    EXPECT_NEAR(tle.epoch.GetDecimalDay(), 264.51782528, 1e-9);

    EXPECT_NEAR(tle.mean_motion, 15.72125391, 1e-9);
    EXPECT_NEAR(tle.mean_motion_first_derivative, -0.00002182, 1e-9);
    EXPECT_NEAR(tle.mean_motion_second_derivative, 0.0, 1e-8);

    EXPECT_NEAR(tle.b_star, -0.11606e-4, 1e-12);
    EXPECT_EQ(tle.ephemeris_type, 0);
    EXPECT_EQ(tle.element_set_number, 292);
    EXPECT_NEAR(tle.inclination, 51.6416, 1e-5);
    EXPECT_NEAR(tle.raan, 247.4627, 1e-5);
    EXPECT_NEAR(tle.eccentricity, 0.0006703, 1e-8);
    EXPECT_NEAR(tle.argument_of_perigee, 130.5360, 1e-5);
    EXPECT_NEAR(tle.mean_anomaly, 325.0288, 1e-5);
    EXPECT_EQ(tle.revolution_number_at_epoch, 56353);
  }

  // Alpha-5.
  // Additionally test empty designator.
  {
    const TLEParser::Result result =
        TLEParser::FromLines(T00000::kLine1, T00000::kLine2);

    EXPECT_TRUE(result.Ok());

    const TLE& tle = result.GetValue();

    EXPECT_EQ(tle.satellite_catalog_number, 270000);

    EXPECT_EQ(tle.international_designator.GetYear(), 0);
    EXPECT_EQ(tle.international_designator.GetYear(), 0);
    EXPECT_EQ(std::string_view(tle.international_designator.GetPiece()), "");
  }
}

}  // namespace astro_core
