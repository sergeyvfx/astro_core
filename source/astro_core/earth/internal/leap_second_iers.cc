// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second_iers.h"

#include "astro_core/parse/arithmetic.h"
#include "astro_core/parse/foreach_line.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// The table is provided in the text form.
//
// The lines starting with a `#` symbol are comments and such lines are
// ignored.
//
// The expected format of the actual data is expected to be:
//
//   <MJD>    <Day> <Month> <Year>    <TAI-UTC>
//
// where
//
//   MJD is the initial Modified Julian Date at which the leap second
//   correction provided at that line is valid. The correction is valid until
//   the epoch given in the next line of the table.
//
//   The MJD indicates the UTC date-time at which the new TAI-UTC value takes
//   affect.
//
//   Day, Month, Year is the UTC date at which the leap second correction in
//   the row is valid from.
//
//   TAI-UTC is the difference in seconds between TAI (International Atomic
//   Time) time and the UTC.
auto LeapSecondIERS::Parse(const std::string_view table_text) -> Result {
  LeapSecondTable table;

  using IntReader = IntReader<int>;
  using FloatReader = FloatReader<double>;

  // TODO(sergey): Find a way to pre-allocate the table. Maybe estimate it from
  // the text length. Or, count the number of new line characters.

  for (const std::string_view line : ForeachLine(table_text)) {
    if (line.empty()) {
      continue;
    }

    if (line.starts_with('#')) {
      // TODO(sergey): Parse expiration date.
      continue;
    }

    // Remainder of unparsed table row string.
    std::string_view remainder = line;

    // Modified Julian Date at which the leap second is introduced.
    const FloatReader::Result mjd = FloatReader::Read(remainder);
    if (!mjd.Ok()) {
      return Result(Error::kError);
    }

    // UTC day, month, year.
    const IntReader::Result day = IntReader::Read(remainder);
    const IntReader::Result month = IntReader::Read(remainder);
    const IntReader::Result year = IntReader::Read(remainder);
    if (!day.Ok() || !month.Ok() || !year.Ok()) {
      return Result(Error::kError);
    }

    // TAI-UTC, in seconds.
    const IntReader::Result tai_minus_utc = IntReader::Read(remainder);
    if (!tai_minus_utc.Ok()) {
      return Result(Error::kError);
    }

    table.AddRow(ModifiedJulianDate(mjd.GetValue()), tai_minus_utc.GetValue());
  }

  table.Preprocess();

  return Result(std::move(table));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
