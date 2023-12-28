// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Geographic position: a longitude/latitude position on the surface of Earth,
// without attachment to an observation time.

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Geodetic;
class QTH;

class Geographic {
 public:
  // Construct default cartesian coordinate.
  // Initializes longitude and latitude to 0.
  Geographic() = default;

  // Initialization from the given parameters.
  //
  // The parameters follow the semantic of the fields of the coordinate.
  //
  // This constructor allows to construct coordinate using designated
  // initializer to improve readability of the meaning of the passed values.
  // The non-specified fields are initialized to their default values.
  //
  // Example:
  //
  //   Geodetic coord({.latitude = lat,
  //                   .longitude = lon});
  struct Parameters {
    double latitude{0};
    double longitude{0};
  };
  explicit constexpr Geographic(const Parameters& parameters)
      : latitude(parameters.latitude), longitude(parameters.longitude) {}

  // Construct the geographic coordinate from the geodetic by projecting it to
  // the surface of earth.
  static auto FromGeodetic(const Geodetic& geodetic) -> Geographic;

  // Construct the geographic coordinate from the QTH locator.
  static auto FromQTH(const QTH& qth) -> Geographic;

  // Construct new geographic coordinate from the current one but replacing the
  // value of a single field.
  constexpr auto WithLatitude(const double new_latitude) const -> Geographic {
    Geographic result = *this;
    result.latitude = new_latitude;
    return result;
  }
  constexpr auto WithLongitude(const double new_longitude) const -> Geographic {
    Geographic result = *this;
    result.longitude = new_longitude;
    return result;
  }

  // Check for an exact equality.
  inline auto operator==(const Geographic& other) {
    return latitude == other.latitude && longitude == other.longitude;
  }
  inline auto operator!=(const Geographic& other) { return !(*this == other); }

  // Geodetic latitude, north/south, radians.
  // Canonically is [-pi/2 .. pi/2], 0 is the equator.
  double latitude{0};

  // Longitude, east/west, radians.
  // Canonically is [-pi .. pi].
  double longitude{0};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
