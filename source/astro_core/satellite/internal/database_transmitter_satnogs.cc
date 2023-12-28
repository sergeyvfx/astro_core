// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/database_transmitter_satnogs.h"

#include "astro_core/base/internal/json.h"
#include "astro_core/satellite/database.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

namespace {

// Get frequency field denoted by the given key.
// If the key does not exist or is not a numeric value 0 is returned (which is
// translated to no communication).
auto GetFrequency(const json& transmitter_json, const char* key) -> int64_t {
  const auto frequency_it = transmitter_json.find(key);
  if (frequency_it == transmitter_json.end()) {
    return 0;
  }

  if (!frequency_it->is_number_integer()) {
    return 0;
  }

  return frequency_it->get<int64_t>();
}

void AddTransmitter(SatelliteDatabase& database, const json& transmitter_json) {
  // Indicator of a missing fields.
  const auto missing = transmitter_json.end();

  const auto alive_it = transmitter_json.find("alive");
  const auto norad_cat_id_it = transmitter_json.find("norad_cat_id");
  const auto description_it = transmitter_json.find("description");

  // Check the required fields exist.
  if (alive_it == missing || norad_cat_id_it == missing ||
      description_it == missing) {
    return;
  }

  // Check data types of required fields.
  if (!norad_cat_id_it->is_number_integer() || !description_it->is_string()) {
    return;
  }

  // Check the transmitter is to be added.
  if (!alive_it->get<bool>()) {
    return;
  }

  SatelliteDAO satellite_dao =
      database.LookupSatelliteByCatalogNumber(norad_cat_id_it->get<int>());
  if (!satellite_dao) {
    return;
  }

  TransmitterDAO transmitter_dao = satellite_dao.AddTransmitter();
  transmitter_dao.SetName(description_it->get<std::string_view>());

  transmitter_dao.SetDownlinkFrequency(
      GetFrequency(transmitter_json, "downlink_low"));
  transmitter_dao.SetUplinkFrequency(
      GetFrequency(transmitter_json, "uplink_low"));
}

}  // namespace

auto LoadSatNOGSTransmitters(SatelliteDatabase& database,
                             const std::string_view text) -> bool {
  json data_json = json::parse(text, nullptr, false);
  if (data_json.is_discarded()) {
    return false;
  }

  for (const json& transmitter_json : data_json) {
    AddTransmitter(database, transmitter_json);
  }

  return true;
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
