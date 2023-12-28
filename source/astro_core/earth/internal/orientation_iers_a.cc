// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// =============================================================================
// Byte-by-byte description of table from ReadMe.finals2000A in Astropy
// -----------------------------------------------------------------------------
//    Bytes Format Units  Label  Explanations
// -----------------------------------------------------------------------------
//   1-   2   I2    ---     year         To get true calendar year, add 1900 for
//                                       MJD<=51543 or add 2000 for MJD>=51544)
//   3-   4   I2    ---     month
//   5-   6   I2    ---     day          of month
//   8-  15   F8.2    d     MJD          fractional Modified Julian Date
//                                       (MJD UTC)
//       17   A1    ---     PolPMFlag_A  IERS (I) or Prediction (P) flag for
//                                       Bull. A polar motion values
//  19-  27   F9.6  arcsec  PM_x_A       Bull. A PM-x
//  28-  36   F9.6  arcsec  e_PM_x_A     error in PM-x (sec. of arc)
//  38-  46   F9.6  arcsec  PM_y_A       Bull. A PM-y (sec. of arc)
//  47-  55   F9.6  arcsec  e_PM_y_A     error in PM-y (sec. of arc)
//       58   A1    ---     UT1Flag_A    IERS (I) or Prediction (P) flag for
//                                       Bull. A UT1-UTC values
//  59-  68   F10.7 s       UT1_UTC_A    Bull. A UT1-UTC (sec. of time)
//  69-  78   F10.7 s       e_UT1_UTC_A  error in UT1-UTC (sec. of time)
//  80-  86   F7.4  ms      LOD_A        Bull. A LOD (msec. of time)
//                                       -- NOT ALWAYS FILLED
//  87-  93   F7.4  ms      e_LOD_A      error in LOD (msec. of time)
//                                       -- NOT ALWAYS FILLED
//       96   A1    ---     NutFlag_A    IERS (I) or Prediction (P) flag for
//                                       Bull. A nutation values
//  98- 106   F9.3  marcsec dX_2000A_A   Bull. A dX wrt IAU2000A Nutation
//                                       Free Core Nutation NOT Removed
// 107- 115   F9.3  marcsec e_dX_2000A_A error in dX (msec. of arc)
// 117- 125   F9.3  marcsec dY_2000A_A   Bull. A dY wrt IAU2000A Nutation
//                                       Free Core Nutation NOT Removed
// 126- 134   F9.3  marcsec e_dY_2000A_A error in dY (msec. of arc)
// 135- 144   F10.6 arcsec  PM_X_B       Bull. B PM-x (sec. of arc)
// 145- 154   F10.6 arcsec  PM_Y_B       Bull. B PM-y (sec. of arc)
// 155- 165   F11.7 s       UT1_UTC_B    Bull. B UT1-UTC (sec. of time)
// 166- 175   F10.3 marcsec dX_2000A_B   Bull. B dX wrt IAU2000A Nutation
// 176- 185   F10.3 marcsec dY_2000A_B   Bull. B dY wrt IAU2000A Nutation
// -----------------------------------------------------------------------------

#include "astro_core/earth/orientation_iers_a.h"

#include <cassert>

#include "astro_core/parse/arithmetic.h"
#include "astro_core/parse/field_parser.h"
#include "astro_core/parse/foreach_line.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

enum class FieldFlag {
  kMissing,    // The field is missing.
  kIERS,       // The field value is from IERS.
  kPredicted,  // The field value os predicted.
};

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

// Parse existence and prediction flag of the group of fields.
// The flag is expected to be at the given 1-based column.
auto ParseFlag(FieldFlag& flag, const std::string_view line, const int column)
    -> bool {
  assert(column >= 1);

  const size_t index = column - 1;
  if (index >= line.size() || line[index] == ' ') {
    flag = FieldFlag::kMissing;
    return true;
  }

  switch (line[index]) {
    case 'I': flag = FieldFlag::kIERS; return true;
    case 'P': flag = FieldFlag::kPredicted; return true;
  }

  return false;
}

// Parse single row of table.
// Returns true on success.
auto ParseRow(Row& row, const std::string_view line) -> bool {
  FieldFlag PolPMFlag_A{FieldFlag::kMissing};
  if (!ParseFlag(PolPMFlag_A, line, 17)) {
    return false;
  }
  if (PolPMFlag_A == FieldFlag::kMissing) {
    // There is no information in the row: only some dates which will be filled
    // in the later versions of the bulletin.
    return false;
  }

  FieldFlag UT1Flag_A{FieldFlag::kMissing};
  if (!ParseFlag(UT1Flag_A, line, 58)) {
    return false;
  }
  if (UT1Flag_A == FieldFlag::kMissing) {
    // Should have been captured by the check above, but maybe format changed.
    // Assume that there are no fields after this flag to be parsed.
    return false;
  }

  // MJD.
  if (!ParseField<Row, &Row::mjd, FloatReader<double>::Convert, 8, 15>(row,
                                                                       line)) {
    return false;
  }

  // Prefer use values from the Bulletin B when they are available, otherwise
  // use value from the Bulletin A. This is because the Bulletin B supposed to
  // have the final values.

  // Polar motion values.
  if (!RunFieldParsers(row,
                       line,
                       TryParseField<Row,
                                     &Row::polar_motion_x,
                                     FloatReader<double>::Convert,
                                     FieldColumns{135, 144},
                                     FieldColumns{19, 27}>,
                       TryParseField<Row,
                                     &Row::polar_motion_y,
                                     FloatReader<double>::Convert,
                                     FieldColumns{145, 154},
                                     FieldColumns{38, 46}>)) {
    return false;
  }

  // UT1-UTC.
  if (!TryParseField<Row,
                     &Row::ut1_minus_utc,
                     FloatReader<double>::Convert,
                     FieldColumns{155, 165},
                     FieldColumns{59, 68}>(row, line)) {
    return false;
  }

  return true;
}

}  // namespace

auto EarthOrientationIERSA::Parse(const std::string_view table_text) -> Result {
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
