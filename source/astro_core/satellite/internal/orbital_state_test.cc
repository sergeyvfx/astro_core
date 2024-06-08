// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/orbital_state.h"

#include "astro_core/satellite/tle.h"
#include "astro_core/satellite/tle_parser.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

////////////////////////////////////////////////////////////////////////////////
// Prediction.

// The prediction result is verified using the SGP4 library from 12 MArch 2020.
//
//   #include <cstdio>
//
//   #include "SGP4.h"
//
//   const char* gLine1 =
//     "1 25544U 98067A   22222.24306052  .00006554  00000+0  12145-3 0  9997";
//   const char* gLine2 =
//     "2 25544  51.6455  68.8017 0005211 102.6998  65.7390 15.50299332353563";
//
//   int main() {
//     elsetrec satrec;
//     double startmfe, stopmfe, deltamin;
//
//     char line1[130], line2[130];
//     strncpy(line1, gLine1, 130);
//     strncpy(line2, gLine2, 130);
//
//     SGP4Funcs::twoline2rv(
//         line1, line2, ' ', ' ', 'i', wgs72, startmfe, stopmfe, deltamin,
//         satrec);
//
//     const double tsince = 969.992851200081531715;
//
//     double r[3], v[3];
//     SGP4Funcs::sgp4(satrec, tsince, r, v);
//
//     printf("r: %.18f %.18f %.18f\n", r[0], r[1], r[2]);
//     printf("v: %.18f %.18f %.18f\n", v[0], v[1], v[2]);
//
//     return 0;
//   }
//
//   $ clang++ -o sgp4 -O2 -fsanitize=address main.cc SGP4.cpp && ./sgp4
//   r: 4357.092619845991976035 4500.439126847555598943 -2645.108425366327537631
//   v: -2.176811755915935453 5.163121595564016175 5.215977759998599694

TEST(OrbitalState, BasicPrediction) {
  // ISS (ZARYA) TLE Obtained from the wayback machine on November 27, 2022.
  // https://web.archive.org/web/20220810114731/https://celestrak.org/NORAD/elements/stations.txt
  const TLEParser::Result tle = TLEParser::FromLines(
      "1 25544U 98067A   22222.24306052  .00006554  00000+0  12145-3 0  9997",
      "2 25544  51.6455  68.8017 0005211 102.6998  65.7390 15.50299332353563");
  EXPECT_TRUE(tle.Ok());

  OrbitalState orbital_state;
  EXPECT_TRUE(orbital_state.InitializeFromTLE(tle.GetValue()));

  const Time time = Time(DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC);

  const OrbitalState::PredictResult predict_result =
      orbital_state.Predict(time);
  ASSERT_TRUE(predict_result.Ok());

  const TEME& teme = *predict_result;

  EXPECT_THAT(Vec3(teme.position.GetCartesian()) / 1000.0,
              Pointwise(DoubleNear(1e-12),
                        {4357.092619845991976035,
                         4500.439126847555598943,
                         -2645.108425366327537631}));

  ASSERT_TRUE(teme.velocity.HasValue());
  EXPECT_THAT(
      Vec3(teme.velocity.GetCartesian()) / 1000.0,
      Pointwise(
          DoubleNear(1e-12),
          {-2.176811755915935453, 5.163121595564016175, 5.215977759998599694}));
}

}  // namespace astro_core
