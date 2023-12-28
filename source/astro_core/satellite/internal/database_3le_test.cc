// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/database_3le.h"

#include "astro_core/satellite/database.h"
#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

TEST(satellite, Load3LE) {
  SatelliteDatabase database;

  // clang-format off
  Load3LE(
    database,
    "NOAA 15                 \r\n"
    "1 25338U 98030A   22353.84630254  .00000161  00000+0  85293-4 0  9996\r\n"
    "2 25338  98.6264  20.7971 0011354 115.1312 245.1047 14.26209908279442\r\n"
  );
  // clang-format on

  SatelliteDAO satellite_dao = database.LookupSatelliteByCatalogNumber(25338);

  ASSERT_TRUE(satellite_dao);

  EXPECT_EQ(satellite_dao.GetCatalogNumber(), 25338);
  EXPECT_EQ(satellite_dao.GetName(), "NOAA 15");
  EXPECT_EQ(satellite_dao.GetTLE().element_set_number, 999);
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
