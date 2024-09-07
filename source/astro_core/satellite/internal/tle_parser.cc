// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/tle_parser.h"

#include <cassert>
#include <type_traits>

#include "astro_core/base/convert.h"
#include "astro_core/base/unreachable.h"
#include "astro_core/parse/arithmetic.h"
#include "astro_core/parse/field_parser.h"
#include "astro_core/satellite/alpha5.h"
#include "astro_core/satellite/internal/tle_float_parser.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

constexpr int kNumColumns = 69;

// Parse a field using IntReader to convert string data to field value.
//
// The field is provided as a pointer to an object member.
// The columns of the line are base-1 indices.
template <class Object, auto Field, int kStartColumn, int kEndColumn>
auto ParseIntField(Object& object, const std::string_view line) -> bool {
  return ParseField<Object,
                    Field,
                    IntReader<int>::Convert,
                    kStartColumn,
                    kEndColumn>(object, line);
}

// Parse a field using IntReader to convert string data to field value.
// If the TLE field is fully empty then the value in the object is left
// unchanged.
//
// The field is provided as a pointer to an object member.
// The columns of the line are base-1 indices.
template <class Object, auto Field, int kStartColumn, int kEndColumn>
auto ParseOptionalIntField(Object& object, const std::string_view line)
    -> bool {
  const std::string_view field_str =
      GetFieldView(line, kStartColumn, kEndColumn);

  if (field_str.find_first_not_of(' ') == std::string_view::npos) {
    return true;
  }

  return ParseField<Object,
                    Field,
                    IntReader<int>::Convert,
                    kStartColumn,
                    kEndColumn>(object, line);
}

// Parse a field using TLE's FloatReader to convert string data to field value.
//
// This parser will deal with optional implicit exponent (whish is denoted by
// just a sign and not the "e" character).
//
// The field is provided as a pointer to an object member.
// The columns of the line are base-1 indices.
template <class Object, auto Field, int kStartColumn, int kEndColumn>
auto ParseFloatField(Object& object, const std::string_view line) -> bool {
  return ParseField<Object,
                    Field,
                    tle_internal::FloatReader<double>::Convert,
                    kStartColumn,
                    kEndColumn>(object, line);
}

// Parse a field using TLE's AssumedDecimalFloatReader to convert string data to
// field value.
//
// The value of the field is assumed to be the decimal symbols, as if there is
// an implicit "0." prefixing the string representation of the value in the
// field.
//
// This parser will deal with optional implicit exponent (whish is denoted by
// just a sign and not the "e" character).
//
// The field is provided as a pointer to an object member.
// The columns of the line are base-1 indices.
template <class Object, auto Field, int kStartColumn, int kEndColumn>
auto ParseAssumedDecimalFloatField(Object& object, const std::string_view line)
    -> bool {
  return ParseField<Object,
                    Field,
                    tle_internal::AssumedDecimalFloatReader<double>::Convert,
                    kStartColumn,
                    kEndColumn>(object, line);
}

// Parser which reads string from the TLE line, strip trailing whitespace, and
// copies characters to the TLE field which must be a bounded character array.
//
// The field is provided as a pointer to an object member.
// The columns of the line are base-1 indices.
template <class Object, auto Field, int kStartColumn, int kEndColumn>
auto ParseCharSequenceField(Object& object, const std::string_view line)
    -> bool {
  constexpr int kFieldStart = kStartColumn - 1;
  constexpr int kFieldLength = kEndColumn - kStartColumn + 1;

  // Check that the field is big enough to hold the requested number of columns,
  // including the null-terminator.
  const size_t kBufferSize = std::size(object.*Field);
  if (kBufferSize <= kFieldLength) {
    return false;
  }

  std::string_view str = line.substr(kFieldStart, kFieldLength);

  // Strip the trailing whitespace.
  const size_t last_char = str.find_last_not_of(' ');
  if (last_char != std::string_view::npos) {
    str = str.substr(0, last_char + 1);
  } else {
    str = str.substr(0, 0);
  }

  // Copy the data to the field, and ensure null-terminator.
  str.copy(std::data(object.*Field), str.size());
  (object.*Field)[str.size()] = '\0';

  return true;
}

// Parse satellite catalog number from the TLE.
// Supports both numeric-only and Alpha-5 notation.
auto ParseSatelliteCatalogNumber(TLE& tle, const std::string_view line)
    -> bool {
  // Parse the trailing 4 digits
  if (!ParseIntField<TLE, &TLE::satellite_catalog_number, 4, 7>(tle, line)) {
    return false;
  }

  const char ch = line[2];
  int prefix = 0;

  if (ch >= '0' && ch <= '9') {
    prefix = ch - '0';
  } else if (ch >= 'A' && ch <= 'Z') {
    if (ch == 'O' || ch == 'I') {
      // The characters O and I are not used to avoid confusion with digits0
      // and 1.
      return false;
    }
    prefix = Alpha5ToNumber(ch);
  } else {
    return false;
  }

  tle.satellite_catalog_number = prefix * 10000 + tle.satellite_catalog_number;

  return true;
}

// Check whether the given classification denoted as charater is known.
constexpr auto IsValidEncodedClassification(const char ch) -> bool {
  return ch == 'U' || ch == 'C' || ch == 'S';
}

// Decode classification from its character representation.
constexpr auto DecodeClassification(const char ch) -> TLE::Classification {
  assert(IsValidEncodedClassification(ch));

  switch (ch) {
    case 'U': return TLE::Classification::kUnclassified;
    case 'C': return TLE::Classification::kClassified;
    case 'S': return TLE::Classification::kSecret;
  }

  Unreachable();
}

auto ParseClassification(TLE& tle, const std::string_view line) -> bool {
  const char ch = line[7];
  if (!IsValidEncodedClassification(ch)) {
    return false;
  }
  tle.classification = DecodeClassification(ch);
  return true;
}

auto ParseDesignator(TLE& tle, const std::string_view line) -> bool {
  struct SimpleDesignator {
    int year{-1};  // Last two digits of launch year.
    int number{0};
    char piece[4]{""};
  };

  // Parse individual fields, as-is.
  SimpleDesignator simple_designator;
  const bool parse_result = RunFieldParsers(
      simple_designator,
      line,
      // Launch year: last two digits.
      ParseOptionalIntField<SimpleDesignator, &SimpleDesignator::year, 10, 11>,
      // Launch number of the year.
      ParseOptionalIntField<SimpleDesignator,
                            &SimpleDesignator::number,
                            12,
                            14>,
      // Piece of the launch.
      ParseCharSequenceField<SimpleDesignator,
                             &SimpleDesignator::piece,
                             15,
                             17>);
  if (!parse_result) {
    return parse_result;
  }

  // Convert the year to the full notation.
  if (simple_designator.year == -1) {
    tle.international_designator.SetYear(0);
  } else if (simple_designator.year < 57) {
    tle.international_designator.SetYear(2000 + simple_designator.year);
  } else {
    tle.international_designator.SetYear(1900 + simple_designator.year);
  }

  tle.international_designator.SetNumber(simple_designator.number);
  tle.international_designator.SetPiece(simple_designator.piece);

  return true;
}

auto ParseEpoch(TLE& tle, const std::string_view line) -> bool {
  struct SimpleEpoch {
    int year{0};
    double decimal_day{0};
  };

  SimpleEpoch simple_epoch;
  const bool parse_result = RunFieldParsers(
      simple_epoch,
      line,
      // Epoch year: last two digits.
      ParseIntField<SimpleEpoch, &SimpleEpoch::year, 19, 20>,
      // Day of the year and fractional portion of the day.
      ParseFloatField<SimpleEpoch, &SimpleEpoch::decimal_day, 21, 32>);

  if (!parse_result) {
    return false;
  }

  if (simple_epoch.year < 57) {
    tle.epoch.SetYear(2000 + simple_epoch.year);
  } else {
    tle.epoch.SetYear(1900 + simple_epoch.year);
  }

  tle.epoch.SetDecimalDay(simple_epoch.decimal_day);

  return true;
}

auto ParseMeanMotion(TLE& tle, const std::string_view line) -> bool {
  return RunFieldParsers(
      tle,
      line,
      // First derivative of mean motion; the ballistic coefficient.
      ParseFloatField<TLE, &TLE::mean_motion_first_derivative, 34, 43>,
      // Second derivative of mean motion.
      ParseAssumedDecimalFloatField<TLE,
                                    &TLE::mean_motion_second_derivative,
                                    45,
                                    52>);
}

auto CommonLineValidate(const int line_number,
                        const std::string_view line,
                        TLEParser::Error& error) -> bool {
  if (line.size() < kNumColumns) {
    error = TLEParser::Error::kInputTruncated;
    return false;
  }

  // Check that the line indeed is the first TLE line.
  if (line[0] != line_number + '0') {
    error = TLEParser::Error::kInvalidFormat;
    return false;
  }

  return true;
}

// Parse the first line of TLE data.
auto ParseLine1(TLE& tle, const std::string_view line, TLEParser::Error& error)
    -> bool {
  if (!CommonLineValidate(1, line, error)) {
    return false;
  }

  if (!RunFieldParsers(tle,
                       line,
                       ParseSatelliteCatalogNumber,
                       ParseClassification,
                       ParseDesignator,
                       ParseEpoch,
                       ParseMeanMotion,
                       // B*
                       ParseAssumedDecimalFloatField<TLE, &TLE::b_star, 54, 61>,
                       // Ephemeris type.
                       ParseIntField<TLE, &TLE::ephemeris_type, 63, 63>,
                       // Element set number
                       ParseIntField<TLE, &TLE::element_set_number, 65, 68>)) {
    error = TLEParser::Error::kInvalidFormat;
    return false;
  }

  return true;
}

// Parse the second line of TLE data.
auto ParseLine2(TLE& tle, const std::string_view line, TLEParser::Error& error)
    -> bool {
  if (!CommonLineValidate(2, line, error)) {
    return false;
  }

  if (!RunFieldParsers(
          tle,
          line,
          // Inclination.
          ParseFloatField<TLE, &TLE::inclination, 9, 16>,
          // Right ascension of the ascending node.
          ParseFloatField<TLE, &TLE::raan, 18, 25>,
          // Eccentricity.
          ParseAssumedDecimalFloatField<TLE, &TLE::eccentricity, 27, 33>,
          // Argument of perigee.
          ParseFloatField<TLE, &TLE::argument_of_perigee, 35, 42>,
          // Mean anomaly.
          ParseFloatField<TLE, &TLE::mean_anomaly, 44, 51>,
          // Mean motion.
          ParseFloatField<TLE, &TLE::mean_motion, 53, 63>,
          // Revolution number at epoch.
          ParseFloatField<TLE, &TLE::revolution_number_at_epoch, 64, 68>)) {
    error = TLEParser::Error::kInvalidFormat;
    return false;
  }

  return true;
}

}  // namespace

auto TLEParser::FromLines(const std::string_view line1,
                          const std::string_view line2) -> Result {
  TLE tle;

  // Incrementally parse every line into the TLE object, checking for the
  // possible errors.

  Error error = Error::kInvalidFormat;

  {
    if (!ParseLine1(tle, line1, error)) {
      return Result(error);
    }
  }

  {
    if (!ParseLine2(tle, line2, error)) {
      return Result(error);
    }
  }

  return Result(std::move(tle));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
