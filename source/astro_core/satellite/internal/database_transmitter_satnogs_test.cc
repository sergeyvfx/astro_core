// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/database_transmitter_satnogs.h"

#include "astro_core/satellite/database.h"
#include "astro_core/unittest/test.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

TEST(satellite, LoadSatNOGSTransmitters) {
  const char* json =
      "[\n"
      "    {\n"
      "        \"uuid\": \"mjsHcYajEgbiS9cbKfecGo\",\n"
      "        \"description\": \"APT\",\n"
      "        \"alive\": true,\n"
      "        \"type\": \"Transmitter\",\n"
      "        \"uplink_low\": null,\n"
      "        \"uplink_high\": null,\n"
      "        \"uplink_drift\": null,\n"
      "        \"downlink_low\": 137620000,\n"
      "        \"downlink_high\": null,\n"
      "        \"downlink_drift\": null,\n"
      "        \"mode\": \"APT\",\n"
      "        \"mode_id\": 44,\n"
      "        \"uplink_mode\": null,\n"
      "        \"invert\": false,\n"
      "        \"baud\": null,\n"
      "        \"sat_id\": \"FSYV-6957-0977-9643-9047\",\n"
      "        \"norad_cat_id\": 25338,\n"
      "        \"norad_follow_id\": null,\n"
      "        \"status\": \"active\",\n"
      "        \"updated\": \"2019-06-15T22:34:13.891380Z\",\n"
      "        \"citation\": \"https://sourceforge.net/p/gpredict/bugs/58/\",\n"
      "        \"service\": \"Unknown\",\n"
      "        \"iaru_coordination\": \"N/A\",\n"
      "        \"iaru_coordination_url\": \"\",\n"
      "        \"itu_notification\":\n"
      "        {\n"
      "            \"urls\":\n"
      "            []\n"
      "        },\n"
      "        \"frequency_violation\": false\n"
      "    }\n"
      "]\n";

  SatelliteDatabase database;
  database.AddSatellite(25338, "NOAA 15");

  EXPECT_TRUE(LoadSatNOGSTransmitters(database, json));

  SatelliteDAO satellite_dao = database.LookupSatelliteByCatalogNumber(25338);
  ASSERT_TRUE(satellite_dao);

  TransmitterDAO transmitter_dao = satellite_dao.GetFirstTransmitter();
  ASSERT_TRUE(transmitter_dao);
  EXPECT_EQ(transmitter_dao.GetName(), "APT");
  EXPECT_EQ(transmitter_dao.GetDownlinkFrequency(), 137620000);
  EXPECT_EQ(transmitter_dao.GetUplinkFrequency(), 0);
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
