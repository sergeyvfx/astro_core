// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/gcrf.h"

#include "astro_core/coordinate/itrf.h"
#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

class GCRFTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

TEST_F(GCRFTest, Construct) {
  const DateTime date_time(2022, 8, 10, 22, 0, 0);
  const Time time{date_time, TimeScale::kUTC};

  const GCRF coordinate(
      {.observation_time{time}, .position{1, 2, 3}, .velocity{{4, 5, 6}}});

  EXPECT_EQ(coordinate.position.cartesian(), Vec3(1, 2, 3));
  ASSERT_TRUE(coordinate.velocity.has_value());
  EXPECT_EQ(coordinate.velocity.cartesian(), Vec3(4, 5, 6));
  EXPECT_EQ(coordinate.observation_time.AsFormat<DateTime>(), date_time);
}

TEST_F(GCRFTest, FromITRF) {
  // Test follows the test suit for ITRFToGCRF().

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_itrf(-2801.428206798944302136,
                    5602.703300938050233526,
                    -2645.094088710325195279);
  const Vec3 v_itrf(
      -5.184234346857372167, -0.137714270932494498, 5.215984905464025267);

  const ITRF itrf(
      {.observation_time = time, .position = r_itrf, .velocity = v_itrf});

  const GCRF gcrf = GCRF::FromITRF(itrf);

  EXPECT_EQ(gcrf.observation_time, itrf.observation_time);

  EXPECT_THAT(Vec3(gcrf.position.cartesian()),
              Pointwise(DoubleNear(1e-12),
                        {4374.025673658524283383,
                         4478.288319286147270759,
                         -2654.739186783237528289}));

  ASSERT_TRUE(gcrf.velocity.has_value());
  EXPECT_THAT(
      Vec3(gcrf.velocity.cartesian()),
      Pointwise(
          DoubleNear(1e-12),
          {-2.139329590299860584, 5.174189009638810788, 5.220516738855706329}));
}

}  // namespace astro_core
