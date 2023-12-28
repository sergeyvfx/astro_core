// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Horizontal coordinate system
// Operates in units of radians and meters.
//
// https://en.wikipedia.org/wiki/Horizontal_coordinate_system

#pragma once

#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ITRF;

class Horizontal {
 public:
  // Construct default cartesian coordinate.
  //
  // All fields are initialized to their default values. For the longitude,
  // latitude, and height it is a zero. For the time it is not specified.
  Horizontal() = default;

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
  //                   .elevation = el,
  //                   .azimuth = az,
  //                   .distance = r});
  struct Parameters {
    Time observation_time{};
    double elevation{0};
    double azimuth{0};
    double distance{0};
  };
  explicit constexpr Horizontal(const Parameters& parameters)
      : elevation(parameters.elevation),
        azimuth(parameters.azimuth),
        distance(parameters.distance),
        observation_time(parameters.observation_time) {}

  // Construct the horizontal coordinate from the given position of the object
  // and observer in thr ITRF.
  // Uses the observation time from the object ITRF.
  static auto FromITRF(const ITRF& itrf, const ITRF& site_itrf) -> Horizontal;

  // Elevation, sometimes referred to as altitude.
  //
  // The angle between the object and the observer's local horizon. For visible
  // objects, it is an angle between 0° and 90°.
  double elevation{0};

  // The angle of the object around the horizon, usually measured from true
  // north and increasing eastward.
  double azimuth{0};

  // Object (satellite) distance from the observer.
  double distance{0};

  // Time at which the position and the velocity has been measured.
  Time observation_time{};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
