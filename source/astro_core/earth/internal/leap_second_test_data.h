// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Data needed for leap second tests.

#pragma once

#include <filesystem>
#include <string>

#include "tl_io/tl_io_file.h"

#include "astro_core/earth/leap_second_iers.h"
#include "astro_core/earth/leap_second_table.h"
#include "astro_core/unittest/test.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace test_data {

// Create a table providing real information about the leap second announcement.
inline auto CreateLeapSecondTable() -> LeapSecondTable {
  using File = tiny_lib::io_file::File;
  using Path = std::filesystem::path;

  const Path table_filename =
      testing::TestFileAbsolutePath(Path("iers") / "Leap_Second.dat");

  std::string table_str;
  if (!File::ReadText(table_filename, table_str)) {
    ADD_FAILURE() << "Error reading " << table_filename;
    return {};
  }

  LeapSecondIERS::Result result = LeapSecondIERS::Parse(table_str);
  if (!result.Ok()) {
    ADD_FAILURE() << "Error parsing " << table_filename;
    return {};
  }

  return std::move(result.GetValue());
}

}  // namespace test_data

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
