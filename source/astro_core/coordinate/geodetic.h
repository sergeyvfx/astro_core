// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Geodetic coordinates.
// Operates in units of radians and meters.
//
// https://en.wikipedia.org/wiki/Geodetic_coordinates

#pragma once

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ITRF;
class Geographic;

class Geodetic {
 public:
  // Construct default cartesian coordinate.
  //
  // All fields are initialized to their default values. For the longitude,
  // latitude, and height it is a zero. For the time it is not specified.
  Geodetic() = default;

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
  //   Geodetic coord({.observation_time = t,
  //                   .latitude = lat,
  //                   .longitude = lon,
  //                   .height = h});
  struct Parameters {
    Time observation_time{};
    double latitude{0};
    double longitude{0};
    double height{0};
  };
  explicit constexpr Geodetic(const Parameters& parameters)
      : observation_time(parameters.observation_time),
        latitude(parameters.latitude),
        longitude(parameters.longitude),
        height(parameters.height) {}

  // Construct the geodetic coordinate by converting the given geocentric
  // coordinates. The unit is expected to be meters.
  static auto FromGeocentric(const Vec3& geocentric,
                             const Time& observation_time) -> Geodetic;

  // Construct the geodetic coordinate by converting the given geographic
  // coordinates. The geographic coordinates is what is used on maps: they only
  // provide latitude and longitude, but not the height.
  //
  // Creates coordinate which is on the surface of the Earth ellipsoid.
  static auto FromGeographic(const Geographic& geographic,
                             const Time& observation_time) -> Geodetic;

  // Construct the geodetic coordinate from the given ITRF frame.
  static auto FromITRF(const ITRF& itrf) -> Geodetic;

  // Construct new geodetic coordinate from the current one but replacing the
  // value of a single field.
  constexpr auto WithLatitude(const double new_latitude) const -> Geodetic {
    Geodetic result = *this;
    result.latitude = new_latitude;
    return result;
  }
  constexpr auto WithLongitude(const double new_longitude) const -> Geodetic {
    Geodetic result = *this;
    result.longitude = new_longitude;
    return result;
  }
  constexpr auto WithHeight(const double new_height) const -> Geodetic {
    Geodetic result = *this;
    result.height = new_height;
    return result;
  }
  constexpr auto WithObservationTime(const Time& new_observation_time) const
      -> Geodetic {
    Geodetic result = *this;
    result.observation_time = new_observation_time;
    return result;
  }

  // Time at which the position and the velocity has been measured.
  Time observation_time{};

  // Geodetic latitude, north/south, radians.
  // Canonically is [-pi/2 .. pi/2], 0 is the equator.
  double latitude{0};

  // Longitude, east/west, radians.
  // Canonically is [-pi .. pi].
  double longitude{0};

  // Ellipsoidal (geodetic) height.
  //
  // The distance between the point of interest and the ellipsoid surface,
  // evaluated along the ellipsoidal normal vector; it is defined as a signed
  // distance, such that points inside the ellipsoid have negative height.
  double height{0};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
