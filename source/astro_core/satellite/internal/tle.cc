// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/tle.h"

#include <algorithm>
#include <cassert>

#include "astro_core/base/ctype.h"
#include "astro_core/base/unreachable.h"
#include "astro_core/math/math.h"
#include "astro_core/satellite/alpha5.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto CalculateTLELineChecksum(const std::span<const char> line) -> int {
  const std::span<const char> line_without_checksum =
      line.subspan<0, TLE::kLineLength - 1>();

  int checksum = 0;
  for (const char ch : line_without_checksum) {
    if (IsDigit(ch)) {
      checksum += ch - '0';
    } else if (ch == '-') {
      checksum += 1;
    }
  }
  return checksum;
}

////////////////////////////////////////////////////////////////////////////////
// Conversion to string.
//
//          1         2         3         4         5
// 1234567890123456789012345678901234567890123456789012345678901234567890
// 1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927
// 2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537

namespace {

// Convert the number to string using the fixed width notation, padding the
// number with zeros on the left. The result is stored in the buffer without
// null-terminator added.
void NumberToBufferFixedWidth(const int number, const std::span<char> buffer) {
  assert(!buffer.empty());

  const size_t width = buffer.size();
  int current_number = number;
  for (size_t i = 0; i < width; ++i) {
    buffer[width - i - 1] = '0' + (current_number % 10);
    current_number /= 10;
  }
}

// Convert the number to string and write it to the buffer aligning characters
// to the right. No null terminator is written.
void NumberToBufferRightAligned(const int number,
                                const std::span<char> buffer) {
  assert(!buffer.empty());

  const size_t width = buffer.size();
  int current_number = number;
  for (size_t i = 0; i < width; ++i) {
    buffer[width - i - 1] = '0' + (current_number % 10);
    current_number /= 10;
    if (current_number == 0) {
      break;
    }
  }
}

// Convert digits of the fractional part of the value to string and write them
// to the given buffer. No sign or decimal point is written to the buffer.
// The entire buffer is filled. No null terminator is written either.
void FractionalToBufferFixedWidth(const double value,
                                  const std::span<char> buffer) {
  const double abs_value = Abs(value);
  const double fractional = abs_value - Trunc(abs_value);

  const size_t width = buffer.size();
  const int fixed_fractional = fractional * Pow(10.0, double(width)) + 0.5;
  NumberToBufferFixedWidth(fixed_fractional, buffer);
}

// Convert floating point with a fixed number of integer and decimal places to
// string and write it to buffer. No null terminator is written.
void FixedUnsignedFloatToBuffer(const double value,
                                const int num_integer_digits,
                                const int num_decimal_places,
                                const std::span<char> buffer) {
  assert(buffer.size() == num_integer_digits + num_decimal_places + 1);
  (void)num_decimal_places;

  const int integer = value;

  NumberToBufferRightAligned(integer, buffer.subspan(0, num_integer_digits));

  buffer[num_integer_digits] = '.';

  FractionalToBufferFixedWidth(value, buffer.subspan(num_integer_digits + 1));
}

// Get power such that 0.1 <= value * Pow(10, power) < 1
auto GetPowerToAssumedDecimal(const double value) -> double {
  assert(value < 1.0);

  int power = 0;
  double current_value = Abs(value);
  while (current_value < 0.1 && power < 9) {
    ++power;
    current_value *= 10;
  }

  if (current_value < 0.1) {
    return 0;
  }

  return power;
}

// Run a set of conversion function, checking that all of them return kOk.
// If one converter returns an error then the conversion is stopped.
template <class Converter>
inline auto RunConverters(const TLE& tle,
                          const std::span<char> buffer,
                          Converter&& converter) -> Error {
  return converter(tle, buffer);
}
template <class Converter, class... Converters>
inline auto RunConverters(const TLE& tle,
                          const std::span<char> buffer,
                          Converter&& converter,
                          Converters... converters) -> Error {
  const Error result = converter(tle, buffer);
  if (result != Error::kOk) {
    return result;
  }

  return RunConverters(tle, buffer, converters...);
}

auto ScientificValueWithAssumedDecimalToBufferFixedWidth(
    const double value, const std::span<char> buffer) -> Error {
  if (value < -1 || value > 1) {
    return Error::kInvalidValue;
  }

  const size_t width = buffer.size();
  assert(width >= 4);

  if (value < 0) {
    // Positive values do not specify sign and the symbol is kept as space.
    buffer[0] = '-';
  }

  const int power = GetPowerToAssumedDecimal(value);
  assert(power < 10);

  const int fixed = Abs(value) * Pow(10.0, double(power + width - 3)) + 0.5;
  NumberToBufferFixedWidth(fixed, buffer.subspan(1, width - 3));

  buffer[width - 2] = '-';
  buffer[width - 1] = '0' + power;

  return Error::kOk;
}

// Write degrees ranging in 0 .. 360 to buffer.
// Performs the range check and returns Error::kInvalidValue if the range is
// violated.
// Writes the value with fixed 3 integer places.
// No null terminator is written.
auto DegreesToBuffer(const double degrees, const std::span<char> buffer)
    -> Error {
  if (degrees < 0 || degrees > 360) {
    return Error::kInvalidValue;
  }

  FixedUnsignedFloatToBuffer(degrees, 3, buffer.size() - 4, buffer);

  return Error::kOk;
}

auto SatelliteCatalogNumberToBuffer(const TLE& tle,
                                    const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<2, 5>();

  const size_t width = field.size();
  int current_number = tle.satellite_catalog_number;
  for (size_t i = 0; i < width - 1 && current_number != 0; ++i) {
    field[width - i - 1] = '0' + (current_number % 10);
    current_number /= 10;
  }

  if (current_number == 0) {
    // No remaining digits to convert, early output.
    return Error::kOk;
  }

  if (current_number < 10) {
    // The digit fits into decimal.
    field[0] = '0' + current_number;
    return Error::kOk;
  }

  field[0] = NumberToAlpha5(current_number);

  return Error::kOk;
}

auto ClassificationToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  buffer[7] = TLE::ClassificationToChar(tle.classification);

  return Error::kOk;
}

auto DesignatorToBuffer(const TLE& tle, const std::span<char> buffer) -> Error {
  const int year = tle.international_designator.GetYear();
  if (year < 0) {
    return Error::kInvalidValue;
  }

  if (year == 0) {
    // Ignore the designator for an analyst object.
    return Error::kOk;
  }

  const std::span<char> field = buffer.subspan<9, 8>();

  NumberToBufferFixedWidth(year % 100, field.subspan<0, 2>());

  const int number = tle.international_designator.GetNumber();
  if (number < 0 || number >= 1000) {
    return Error::kInvalidValue;
  }
  NumberToBufferFixedWidth(number, field.subspan<2, 3>());

  const std::string_view piece = tle.international_designator.GetPiece();
  if (piece.size() > 3) {
    return Error::kInvalidValue;
  }
  piece.copy(field.data() + 5, 3);

  return Error::kOk;
}

auto EpochToBuffer(const TLE& tle, const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<18, 14>();

  const int year = tle.epoch.GetYear();
  if (year < 0) {
    return Error::kInvalidValue;
  }
  NumberToBufferFixedWidth(year % 100, field.subspan<0, 2>());

  const double decimal_day = tle.epoch.GetDecimalDay();
  if (decimal_day < 0 || decimal_day > 366) {
    return Error::kInvalidValue;
  }
  FixedUnsignedFloatToBuffer(decimal_day, 3, 8, field.subspan<2, 12>());

  return Error::kOk;
}

auto FirstMotionDerivativeToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  const std::span<char> field = buffer.subspan<33, 10>();

  if (tle.mean_motion_first_derivative > 1 ||
      tle.mean_motion_first_derivative < -1) {
    return Error::kInvalidValue;
  }

  if (tle.mean_motion_first_derivative < 0) {
    // Positive values do not specify sign and the symbol is kept as space.
    field[0] = '-';
  }

  field[1] = '.';  // Decimal point.

  FractionalToBufferFixedWidth(tle.mean_motion_first_derivative,
                               field.subspan<2, 8>());

  return Error::kOk;
}

auto SecondMotionDerivativeToBuffer(const TLE& tle,
                                    const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<44, 8>();

  return ScientificValueWithAssumedDecimalToBufferFixedWidth(
      tle.mean_motion_second_derivative, field.subspan<0, 8>());
}

auto BStarToBuffer(const TLE& tle, const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<53, 8>();

  return ScientificValueWithAssumedDecimalToBufferFixedWidth(
      tle.b_star, field.subspan<0, 8>());
}

auto EphemerisTypeToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  if (tle.ephemeris_type < 0 || tle.ephemeris_type >= 10) {
    return Error::kInvalidValue;
  }

  buffer[62] = '0' + tle.ephemeris_type;

  return Error::kOk;
}

auto ElementSetNumberToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  const std::span<char> field = buffer.subspan<64, 4>();

  if (tle.element_set_number < 0 || tle.element_set_number >= 10000) {
    return Error::kInvalidValue;
  }

  NumberToBufferRightAligned(tle.element_set_number, field);

  return Error::kOk;
}

auto InclinationToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  const std::span<char> field = buffer.subspan<8, 8>();
  return DegreesToBuffer(tle.inclination, field);
}

auto RAANToBuffer(const TLE& tle, const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<17, 8>();
  return DegreesToBuffer(tle.raan, field);
}

auto EccentricityToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  const std::span<char> field = buffer.subspan<26, 7>();

  if (tle.eccentricity < 0 || tle.eccentricity >= 1) {
    return Error::kInvalidValue;
  }

  FractionalToBufferFixedWidth(tle.eccentricity, field);

  return Error::kOk;
}

auto ArgumentOfPerigeeToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  const std::span<char> field = buffer.subspan<34, 8>();
  return DegreesToBuffer(tle.argument_of_perigee, field);
}

auto MeanAnomalyToBuffer(const TLE& tle, const std::span<char> buffer)
    -> Error {
  const std::span<char> field = buffer.subspan<43, 8>();
  return DegreesToBuffer(tle.mean_anomaly, field);
}

auto MeanMotionToBuffer(const TLE& tle, const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<52, 11>();

  if (tle.mean_motion < 0 || tle.mean_motion >= 100) {
    return Error::kInvalidValue;
  }

  FixedUnsignedFloatToBuffer(tle.mean_motion, 2, 8, field);

  return Error::kOk;
}

auto RevolutionNumberAtEpochToBuffer(const TLE& tle,
                                     const std::span<char> buffer) -> Error {
  const std::span<char> field = buffer.subspan<63, 5>();

  if (tle.revolution_number_at_epoch < 0 ||
      tle.revolution_number_at_epoch >= 100000) {
    return Error::kInvalidValue;
  }

  NumberToBufferRightAligned(tle.revolution_number_at_epoch, field);

  return Error::kOk;
}

auto TLEChecksumToBuffer(const TLE& /*tle*/, const std::span<char> buffer)
    -> Error {
  assert(buffer.size() >= TLE::kLineLength);

  buffer[TLE::kLineLength - 1] = '0' + CalculateTLELineChecksum(buffer) % 10;

  return Error::kOk;
}

}  // namespace

auto TLE::FirstLineToBufferUnchecked(std::span<char> buffer) const -> Error {
  assert(buffer.size() >= kLineLength);

  std::fill(buffer.begin(), buffer.end(), ' ');

  buffer[0] = '1';  // Line number.

  RunConverters(*this,
                buffer,
                SatelliteCatalogNumberToBuffer,
                ClassificationToBuffer,
                DesignatorToBuffer,
                EpochToBuffer,
                FirstMotionDerivativeToBuffer,
                SecondMotionDerivativeToBuffer,
                BStarToBuffer,
                EphemerisTypeToBuffer,
                ElementSetNumberToBuffer,
                TLEChecksumToBuffer);

  return Error::kOk;
}

auto TLE::SecondLineToBufferUnchecked(std::span<char> buffer) const -> Error {
  assert(buffer.size() >= kLineLength);

  std::fill(buffer.begin(), buffer.end(), ' ');

  buffer[0] = '2';  // Line number.

  return RunConverters(*this,
                       buffer,
                       SatelliteCatalogNumberToBuffer,
                       InclinationToBuffer,
                       RAANToBuffer,
                       EccentricityToBuffer,
                       ArgumentOfPerigeeToBuffer,
                       MeanAnomalyToBuffer,
                       MeanMotionToBuffer,
                       RevolutionNumberAtEpochToBuffer,
                       TLEChecksumToBuffer);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
