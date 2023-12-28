// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Prediction of visible pass of satellite.
//
// The visibility is calculated relative to an observer coordinate in ITRF.
// The satellite is considered visible when its elevation is above 0.

#pragma once

#include <optional>
#include <ostream>

#include "astro_core/coordinate/itrf.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class OrbitalState;
class Time;
class ITRF;

struct SatellitePass {
  // The satellite's elevation is never above the required minimum throughout
  // the prediction time window.
  bool is_never_visible{false};

  // The satellite is always visible throughout the prediction time window.
  bool is_always_visible{false};

  // Time of acquisition of signal (AOS) and loss of signal (LOS) of a
  // satellite.
  //
  // The AOS is considered to be the time when the satellite raises above an
  // observer's horizon, and LOS is the time when the satellite passes below the
  // horizon.
  //
  // The horizon is calculated assuming the Earth is a smooth ellipsoid without
  // mountains. In practice this means that the figures are just an
  // approximation, and actual AOS/LOS will depend on the observer's elevation
  // and surrounding landscape.
  //
  // Set to nullopt when the event is not know: for example, the satellite is
  // visible at the beginning of the prediction time window, and does not go
  // below horizon during the window. The LOS in this case will be nullopt.
  std::optional<Time> aos;
  std::optional<Time> los;

  // Maximum elevation of the satellite during the pass, in radians.
  double max_elevation{0};
};

auto operator<<(std::ostream& os, const SatellitePass& pass) -> std::ostream&;

struct PredictPassOptions {
  ITRF site_position{};

  // Minimum satellite elevation (in radians) at which the satellite is
  // considered to be visible.
  double min_elevation{0};

  // The number of days to look forward for the prediction.
  //
  // Due to the nature of the orbital motion model the accuracy will degrade
  // with longer the prediction period becomes.
  //
  // This is also the number of days to look backward for AOS in cases when the
  // satellite is visible at the start time of prediction.
  int num_days_to_predict{7};
};

// Get prediction of the currently visible pass, or the next visible pass.
//
// If the satellite is visible at the start_time then its AOS will be looked
// back, and LOS will be looked forward.
//
// If the satellite is not visible at the start_time a first AOS after the
// start_time is predicted (with its corresponding LOS).
auto PredictCurrentOrNextPass(const PredictPassOptions& options,
                              const OrbitalState& orbital_state,
                              const Time& start_time) -> SatellitePass;

// Get prediction of the next visible pass.
// The AOS of the result (if exists) is always greater or equal to the given
// start time.
auto PredictNextPass(const PredictPassOptions& options,
                     const OrbitalState& orbital_state,
                     const Time& start_time) -> SatellitePass;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
