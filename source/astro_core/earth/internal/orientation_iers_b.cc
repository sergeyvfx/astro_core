// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// =============================================================================
// Byte-by-byte description of table from ReadMe.eopc04_IAU2000 in Astropy
// -----------------------------------------------------------------------------
//    Bytes Format Units  Label  Explanations
// -----------------------------------------------------------------------------
//   1-  4   I4    ---     year         Calendar year
//   5-  8   I4    ---     month        Month
//   9- 12   I4    ---     day          day of month (0 hr UTC)
//  13- 19   I7    d       MJD          Modified Julian Date (MJD, 0 hr UTC)
//  20- 30   F11.6 arcsec  PM_x         polar motion x
//  31- 41   F11.6 arcsec  PM_y         polar motion y
//  42- 53   F12.7 s       UT1_UTC      Difference UT1-UTC
//  54- 65   F12.7 s       LOD          length of day
//  66- 76   F11.6 arcsec  dX_2000A     dX wrt IAU2000A Nutation
//  77- 87   F11.6 arcsec  dY_2000A     dY wrt IAU2000A Nutation
//  88- 98   F11.6 arcsec  e_PM_x       error in PM_x
//  99-109   F11.6 arcsec  e_PM_y       error in PM_y
// 110-120   F11.7 s       e_UT1_UTC    error in UT1_UTC
// 121-131   F11.7 s       e_LOD        error in length of day
// 132-143   F12.6 arcsec  e_dX_2000A   error in dX_2000A
// 144-155   F12.6 arcsec  e_dY_2000A   error in dY_2000A
// -----------------------------------------------------------------------------

#include "astro_core/earth/orientation_iers_b.h"

#include <cassert>

#include "astro_core/base/ctype.h"
#include "astro_core/parse/arithmetic.h"
#include "astro_core/parse/field_parser.h"
#include "astro_core/parse/foreach_line.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

// Parsed row of table.
//
// Contains only information needed by the earth orientation table
// implementation. All the fields of the row are valid.
//
// This is almost like Table::Row but the fields are optimized for the parsing
// purposes.
struct Row {
  // Modified Julian date (MJD) in UTC scale.
  double mjd{0};

  // A polar motion values.
  double polar_motion_x{0};
  double polar_motion_y{0};

  // UT1-UTC, is seconds.
  //
  // A value from columns corresponding to Bulletin B is used when available,
  // otherwise Bulletin A is used.
  double ut1_minus_utc{0};
};

// Parse single row of table.
// Returns true on success.
auto ParseRow(Row& row, const std::string_view line) -> bool {
  if (line.empty()) {
    return false;
  }

  // Skip the table header.
  if (!IsDigit(line[0])) {
    return 0;
  }

  // MJD.
  if (!ParseField<Row, &Row::mjd, FloatReader<double>::Convert, 13, 19>(row,
                                                                        line)) {
    return false;
  }

  // Polar motion values.
  if (!RunFieldParsers(row,
                       line,
                       ParseField<Row,
                                  &Row::polar_motion_x,
                                  FloatReader<double>::Convert,
                                  20,
                                  30>,
                       ParseField<Row,
                                  &Row::polar_motion_y,
                                  FloatReader<double>::Convert,
                                  31,
                                  41>)) {
    return false;
  }

  // UT1-UTC.
  if (!ParseField<Row,
                  &Row::ut1_minus_utc,
                  FloatReader<double>::Convert,
                  42,
                  53>(row, line)) {
    return false;
  }

  return true;
}

}  // namespace

auto EarthOrientationIERSB::Parse(const std::string_view table_text) -> Result {
  EarthOrientationTable table;

  // TODO(sergey): Find a way to pre-allocate the table. Maybe estimate it from
  // the text length. Or, count the number of new line characters.

  for (const std::string_view line : ForeachLine(table_text)) {
    Row row;
    if (!ParseRow(row, line)) {
      continue;
    }

    table.AddRow(ModifiedJulianDate(row.mjd),
                 Vec2(row.polar_motion_x, row.polar_motion_y),
                 row.ut1_minus_utc);
  }

  table.Preprocess();

  return Result(std::move(table));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
