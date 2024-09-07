// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Two-line element set (TLE)
//
// This is a data format encoding a list of orbital elements of an
// Earth-orbiting object for a given point in time. The TLE data representation
// is specific to the simplified perturbations models (SGP, SGP4, SDP4, SGP8 and
// SDP8).
//
// Detailed format with exact decimal points placement is available via the
// ADCOM DO Form 12 [1].
//
// References:
//
//   NORAD Two-Line Element Set Format
//   https://celestrak.org/NORAD/documentation/tle-fmt.php
//
//   Two-line element set
//   https://en.wikipedia.org/wiki/Two-line_element_set
//
//   ADCOM DO Form 12
//   https://celestrak.org/NORAD/documentation/ADCOM%20DO%20Form%2012.pdf

#pragma once

#include <string>
#include <string_view>

#include "astro_core/base/error.h"
#include "astro_core/base/unreachable.h"
#include "astro_core/satellite/international_designator.h"
#include "astro_core/time/format/year_decimal_day.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// NOTE: The values are stored in the same units as used in TLE. They might not
// be directly usable by the prediction models.
class TLE {
 public:
  // Length of a single line.
  static inline constexpr size_t kLineLength = 69;

  enum class Classification {
    kUnclassified,
    kClassified,
    kSecret,
  };

  // Satellite catalog number.
  //
  // Always stored as a whole decoded numbers. This means that if TLE provides
  // catalog number of T00000 in the Alpha-5 notation this value is 270000.
  int satellite_catalog_number{0};

  Classification classification;
  InternationalDesignator international_designator;

  // The epoch is typically considered to be in UTC:
  //   https://space.stackexchange.com/a/13826
  YearDecimalDay epoch;

  // Mean motion.
  double mean_motion{0};                   // Revolutions per day,
  double mean_motion_first_derivative{0};  // The ballistic coefficient
  double mean_motion_second_derivative{0};

  // The drag term, or radiation pressure coefficient.
  double b_star{0};

  // Ephemeris type (always zero; only used in undistributed TLE data).
  int ephemeris_type{0};

  // Incremented when a new TLE is generated for this space object.
  int element_set_number{0};

  double inclination{0};  // Degrees.

  // Right ascension of ascending node.
  double raan{0};  // Degrees.

  double eccentricity{0};
  double argument_of_perigee{0};      // Degrees.
  double mean_anomaly{0};             // Degrees.
  int revolution_number_at_epoch{0};  // Revolutions.

  // Convert classification enumerator to character.
  static auto ClassificationToChar(const Classification classification)
      -> char {
    switch (classification) {
      case Classification::kUnclassified: return 'U';
      case Classification::kClassified: return 'C';
      case Classification::kSecret: return 'S';
    }
    Unreachable();
  }

  // Get string representation of the lines of this TLE.
  // The format follows the TLE data format. The length of the result is
  // kLineLength characters not counting the null terminator.
  //
  // Ignores possible errors during conversion. If they occur the result is
  // undefined.
  auto FirstLineAsString() const -> std::string {
    std::string result;
    FirstLineToString(result);
    return result;
  }
  auto SecondLineAsString() const -> std::string {
    std::string result;
    SecondLineToString(result);
    return result;
  }

  // Get string representation of the lines of the TLE. The result is stored in
  // the given buffer.
  //
  // The buffer is to be at least kLineLength characters big. The buffer can
  // be bigger. No null pointer is written to the buffer.
  //
  // Returns true if the conversion succeeded. The conversion might fail if the
  // given buffer is not big enough.
  //
  // Returns the error code of conversion:
  //  - Error::kOk if the designator has been fully written to the buffer
  //  - Error::kResourceExhausted if the buffer is too small to hold the
  //    designator.
  //  - kInvalidValue if a TLE parameter detected to not fit into the TLE field.
  //    For example, if a field with assumed decimal point is actually above 1.
  inline auto FirstLineToBuffer(std::span<char> buffer) const -> Error {
    if (buffer.size() < kLineLength) {
      return Error::kResourceExhausted;
    }
    return FirstLineToBufferUnchecked(buffer);
  }
  inline auto SecondLineToBuffer(std::span<char> buffer) const -> Error {
    if (buffer.size() < kLineLength) {
      return Error::kResourceExhausted;
    }
    return SecondLineToBufferUnchecked(buffer);
  }

  // Get string representation of the lines of the TLE. The result is stored in
  // the given string-line object.
  //
  // The StringType is to implement std::string-style of resize() and a
  // conversion to a span of char. This function does not write the null
  // terminator: the string type is expected to take care of the termination in
  // its resize() method.
  //
  // The type is to support allocation of kLineLength characters.
  //
  // Returns the error code of conversion:
  //  - Error::kOk if the designator has been fully written to the buffer
  //  - Error::kResourceExhausted if the buffer is too small to hold the
  //    designator.
  template <class StringType>
  inline auto FirstLineToString(StringType& str) const -> Error {
    str.resize(kLineLength);
    return FirstLineToBuffer(str);
  }
  template <class StringType>
  inline auto SecondLineToString(StringType& str) const -> Error {
    str.resize(kLineLength);
    return SecondLineToBuffer(str);
  }

 private:
  // The actual implementation of conversion into a buffer which assumes that
  // the buffer is big enough and does not perform extra checks.
  //
  // Returns the error code of conversion:
  //  - Error::kOk if the designator has been fully written to the buffer
  //  - Error::kResourceExhausted if the buffer is too small to hold the
  //    designator.
  auto FirstLineToBufferUnchecked(std::span<char> buffer) const -> Error;
  auto SecondLineToBufferUnchecked(std::span<char> buffer) const -> Error;
};

// Calculate checksum of the given line of TLE.
//
// The checksums for each line are calculated by adding all numerical digits on
// that line, including the line number. One is added to the checksum for each
// negative sign (-) on that line. All other non-digit characters are ignored.
//
// Only uses 68 first characters of the line, so that it is possible to pass
// line with a checksum stored in it.
auto CalculateTLELineChecksum(const std::span<const char> line) -> int;
inline auto CalculateTLELineChecksum(std::string_view line) -> int {
  return CalculateTLELineChecksum(std::span(line.data(), line.length()));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
