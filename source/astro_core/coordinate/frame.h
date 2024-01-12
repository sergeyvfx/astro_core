// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// Base class for frame defining an object position and velocity measured at a
// specific observation time.

#pragma once

#include "astro_core/coordinate/representation_view.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Time;

// Coordinate frame for observations for which only object position is known.
//
// The position is always known and position.has_value() will return true.
//
// Representation defines the native representation of position.
template <class Representation>
class PositionFrame {
  using PositionRepresentationView = MutableRepresentationView<Representation>;

 public:
  // Construct default cartesian coordinate.
  //
  // All fields are initialized to their default values. Position is initialized
  // a zero vector. Time is initialized to a zero point in an unspecified scale.
  PositionFrame() = default;

  // Redefine copy and move construction ans assignments in a way that only
  // data is copied, but not the view. The view always points to the data of
  // its owner object.
  PositionFrame(const PositionFrame& other)
      : observation_time(other.observation_time), position_(other.position_) {}
  PositionFrame(PositionFrame&& other) noexcept
      : observation_time(other.observation_time), position_(other.position_) {}
  auto operator=(const PositionFrame& other) -> PositionFrame& {
    observation_time = other.observation_time;
    position_ = other.position_;
    return *this;
  }
  auto operator=(PositionFrame&& other) -> PositionFrame& {
    observation_time = other.observation_time;
    position_ = other.position_;
    return *this;
  }

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
    Representation position{};
  };
  explicit PositionFrame(const Parameters& parameters) {
    this->observation_time = parameters.observation_time;
    this->position = parameters.position;
  }

  // Time of observation: time at which the position and the velocity have been
  // measured.
  Time observation_time{};

  // View at the position of the object.
  PositionRepresentationView position{position_};

 private:
  // The actual storage of the position.
  Representation position_{};
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
// Representation defines the native representation of position, and
// DifferentialRepresentation defines the native representation of velocity.
template <class Representation, class DifferentialRepresentation>
class PositionVelocityFrame : public PositionFrame<Representation> {
  using BaseClass = PositionFrame<Representation>;

  using VelocityRepresentation = std::optional<DifferentialRepresentation>;
  using VelocityRepresentationView =
      MutableRepresentationView<VelocityRepresentation>;

 public:
  // Construct default cartesian coordinate.
  //
  // All fields are initialized to their default values. Position and velocity
  // are initialized a zero vector. Time is initialized to a zero point in an
  // unspecified scale.
  PositionVelocityFrame() = default;

  // Redefine copy and move construction ans assignments in a way that only
  // data is copied, but not the view. The view always points to the data of
  // its owner object.
  PositionVelocityFrame(const PositionVelocityFrame& other)
      : BaseClass(other), velocity_(other.velocity_) {}
  PositionVelocityFrame(PositionVelocityFrame&& other) noexcept
      : BaseClass(other), velocity_(other.velocity_) {}
  auto operator=(const PositionVelocityFrame& other) -> PositionVelocityFrame& {
    BaseClass::operator=(other);
    velocity_ = other.velocity_;
    return *this;
  }
  auto operator=(PositionVelocityFrame&& other) -> PositionVelocityFrame& {
    BaseClass::operator=(other);
    velocity_ = other.velocity_;
    return *this;
  }

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
    Representation position{};
    VelocityRepresentation velocity{};
  };
  explicit PositionVelocityFrame(const Parameters& parameters) {
    this->observation_time = parameters.observation_time;
    this->position = parameters.position;
    this->velocity = parameters.velocity;
  }

  // View at the velocity of the object.
  VelocityRepresentationView velocity{velocity_};

 private:
  // The actual storage of the velocity.
  VelocityRepresentation velocity_{};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
