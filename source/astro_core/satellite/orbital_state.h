// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Orbital state of satellites relative to the Earth-centered inertial
// coordinate system.

#pragma once

#include "astro_core/base/result.h"
#include "astro_core/coordinate/teme.h"
#include "astro_core/satellite/internal/sgp4/SGP4.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class TLE;

class OrbitalState {
 public:
  enum class Error {
    kError,  // Generic error.

    kMeanElementsRange,     // mean elements, ecc >= 1.0 or ecc < -0.001 or a <
                            // 0.95
    kMeanMotionRange,       // mean motion less than 0.0
    kPortElementsRange,     // pert elements, ecc < 0.0  or  ecc > 1.0
    kSemiLatusRectumRange,  // semi-latus rectum < 0.0
    kSuborbitalEpochElements,  // epoch elements are sub-orbital
    kSatelliteDecayed,         // satellite has decayed

  };

  using PredictResult = Result<TEME, Error>;

  OrbitalState() = default;

  // Initialize the initial state of the model using data from TLE.
  //
  // Returns true on success, false otherwise. The initialization might fail if
  // the physical model can not be initialized from the given TLE: for example,
  // if the parameters are outside of their expected range.
  auto InitializeFromTLE(const TLE& tle) -> bool;

  // Predict the position and velocity of this satellite at the given time.
  auto Predict(const Time& time) const -> PredictResult;

 private:
  // Internal state used for the SGP4 model.
  sgp_internal::elsetrec sgp4_satrec_{};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
