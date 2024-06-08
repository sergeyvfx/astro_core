// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// Base class for frame defining an object position and velocity measured at a
// specific observation time.
// The frame stores position and velocity in the given type, but allows an easy
// conversion between different representation types.

#pragma once

#include <optional>

#include "astro_core/coordinate/representation.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Time;

// Coordinate frame for observations for which only object position is known.
//
// The position is always known and position.has_value() will return true.
//
// PositionType defines the native type in which the position is stored.
template <class PositionType>
class PositionFrame {
 public:
  // Construct default cartesian coordinate.
  //
  // All fields are initialized to their default values. Position is initialized
  // a zero vector. Time is initialized to a zero point in an unspecified scale.
  PositionFrame() = default;

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
  //   Frame coord({.observation_time = t, .position = r});
  struct Parameters {
    Time observation_time{};
    PositionType position{};
  };
  explicit PositionFrame(const Parameters& parameters) {
    this->observation_time = parameters.observation_time;
    this->position = parameters.position;
  }

  // Time of observation: time at which the position and the velocity have been
  // measured.
  Time observation_time{};

  // Position of the object, stored in a type which is makes it easy to convert
  // between representations.
  Representation<PositionType> position;
};

// Coordinate frame for observations for which object position is known, and
// optionally velocity as well.
//
// The position is always known and position.has_value() will return true.
// Before accessing the velocity's value it is required to check it has value:
//
//   if (frame.velocity.has_value()) {
//     std::cout << "Velocity: " << frame.velocity.cartesian() << std::endl;
//   }
//
// PositionType defines the native type in which the position is stored, and
// DifferentialType defines the native type in which the velocity is stored.
template <class PositionType, class DifferentialType>
class PositionVelocityFrame : public PositionFrame<PositionType> {
 public:
  // Construct default cartesian coordinate.
  //
  // All fields are initialized to their default values. Position and velocity
  // are initialized a zero vector. Time is initialized to a zero point in an
  // unspecified scale.
  PositionVelocityFrame() = default;

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
  //   Frame coord({.observation_time = t, .position = r, .velocity = v});
  struct Parameters {
    Time observation_time{};
    PositionType position{};
    std::optional<DifferentialType> velocity{};
  };
  explicit PositionVelocityFrame(const Parameters& parameters) {
    this->observation_time = parameters.observation_time;
    this->position = parameters.position;
    this->velocity = parameters.velocity;
  }

  // Velocity of an object, stored in a type which is makes it easy to convert
  // between representations.
  Representation<std::optional<DifferentialType>> velocity;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
