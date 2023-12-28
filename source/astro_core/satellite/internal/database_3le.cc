// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/database_3le.h"

#include <array>

#include "astro_core/parse/foreach_line.h"
#include "astro_core/satellite/database.h"
#include "astro_core/satellite/tle_parser.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

namespace {

// Trim name from the first line of 3LE.
// Strip pall trailing whitespace, which can be seen in the database provided by
// CelesTrak.
auto TrimName(const std::string_view line1) -> std::string_view {
  const size_t pos = line1.find_last_not_of(' ');
  if (pos == std::string_view::npos) {
    return line1;
  }

  return line1.substr(0, pos + 1);
}

auto Parse3LEAndAddToDatabase(SatelliteDatabase& database,
                              const std::array<std::string_view, 3> lines,
                              const bool check_existing) -> bool {
  const std::string_view name = TrimName(lines[0]);

  const TLEParser::Result result = TLEParser::FromLines(lines[1], lines[2]);
  if (!result.Ok()) {
    return false;
  }

  const TLE& tle = result.GetValue();

  SatelliteDAO satellite_dao;

  if (check_existing) {
    satellite_dao =
        database.LookupSatelliteByCatalogNumber(tle.satellite_catalog_number);
  }

  if (!satellite_dao) {
    satellite_dao = database.AddSatellite(tle.satellite_catalog_number, name);
  }

  satellite_dao.SetTLE(tle);

  return true;
}

}  // namespace

auto Load3LE(SatelliteDatabase& database, const std::string_view text) -> bool {
  bool result = true;

  // If the database is currently empty can skip lookup of satellites for
  // update, as there will be none.
  const bool check_existing = !database.IsEmpty();

  std::array<std::string_view, 3> lines;
  int current_line_index = 0;

  for (const std::string_view line : ForeachLine(text)) {
    lines[current_line_index++] = line;

    if (current_line_index == 3) {
      if (!Parse3LEAndAddToDatabase(database, lines, check_existing)) {
        result = false;
      }
      current_line_index = 0;
    }
  }

  (void)database;

  return result;
}

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
