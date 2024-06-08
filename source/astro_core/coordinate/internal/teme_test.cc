// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/teme.h"

#include "astro_core/coordinate/itrf.h"
#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

class TEMETest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

TEST_F(TEMETest, Construct) {
  const DateTime date_time(2022, 8, 10, 22, 0, 0);
  const Time time{date_time, TimeScale::kUTC};

  const TEME coordinate(
      {.observation_time{time}, .position{1, 2, 3}, .velocity{{4, 5, 6}}});

  EXPECT_EQ(coordinate.position.GetCartesian(), Vec3(1, 2, 3));
  ASSERT_TRUE(coordinate.velocity.HasValue());
  EXPECT_EQ(coordinate.velocity.GetCartesian(), Vec3(4, 5, 6));
  EXPECT_EQ(coordinate.observation_time.AsFormat<DateTime>(), date_time);
}

TEST_F(TEMETest, FromITRF) {
  // Test follows the test suit for ITRFToTEME().

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_itrf{-2801.428206798944302136,
                    5602.703300938050233526,
                    -2645.094088710325195279};
  const Vec3 v_itrf{
      -5.184234346857372167, -0.137714270932494498, 5.215984905464025267};

  const ITRF itrf(
      {.observation_time = time, .position = r_itrf, .velocity = v_itrf});

  const TEME teme = TEME::FromITRF(itrf);

  EXPECT_EQ(teme.observation_time, itrf.observation_time);

  EXPECT_THAT(
      Vec3(teme.position.GetCartesian()),
      Pointwise(DoubleNear(1e-12),
                {4357.092619856639, 4500.439126822302, -2645.108425391841}));

  ASSERT_TRUE(teme.velocity.HasValue());
  EXPECT_THAT(
      Vec3(teme.velocity.GetCartesian()),
      Pointwise(DoubleNear(1e-12),
                {-2.1768117558889037, 5.163121595591936, 5.215977759982141}));
}

}  // namespace astro_core
