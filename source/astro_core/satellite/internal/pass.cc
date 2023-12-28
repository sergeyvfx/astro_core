// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/pass.h"

#include <cassert>
#include <optional>

#include "astro_core/coordinate/horizontal.h"
#include "astro_core/coordinate/teme.h"
#include "astro_core/math/math.h"
#include "astro_core/satellite/orbital_state.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/time_difference.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto operator<<(std::ostream& os, const SatellitePass& pass) -> std::ostream& {
  if (pass.is_always_visible) {
    os << "Always Visible Pass, Max Elevation: "
       << RadiansToDegrees(pass.max_elevation) << " degrees.";
  } else if (pass.is_never_visible) {
    os << "Never Visible Pass";
  } else {
    if (pass.aos) {
      os << "AOS: " << pass.aos->AsFormat<DateTime>();
    } else {
      os << "No AOS";
    }

    if (pass.los) {
      os << ", LOS: " << pass.los->AsFormat<DateTime>();
    } else {
      os << ", No LOS";
    }

    os << ", Max Elevation: " << RadiansToDegrees(pass.max_elevation)
       << " degrees";
  }

  return os;
}

namespace {

// Time step when looking for an approximate value of AOS or LOS.
//
// Small enough value to not miss the entire pass, but big enough to be able to
// quickly look through wide prediction time windows.
constexpr auto kApproximateTimeStep = TimeDifference::FromSeconds(240);

// TIme step used when AOS or LOS is refined from their approximate values.
constexpr auto kRefineTimeStep = TimeDifference::FromSeconds(1);

// The maximum number of steps which will be performed when looking for a
// refined value of AOS or LOW.
constexpr int kRefineMaxSteps = int(
    (kApproximateTimeStep.InSeconds() / kRefineTimeStep.InSeconds()).GetHi());

// Calculate the number of steps which can be done to cover the time window with
// the time steps of given duration.
auto GetNumPredictionSteps(const TimeDifference& time_window,
                           const TimeDifference& time_step) -> int {
  return int(Trunc(time_window.InSeconds() / time_step.InSeconds()).GetHi());
}

// Calculate elevation of satellite over horizon of an observer with the given
// site coordinate at a given time.
// If prediction is not possible then nullopt is returned.
auto CalculateElevationAtTime(const ITRF& site_position,
                              const OrbitalState& orbital_state,
                              const Time& time) -> std::optional<double> {
  const OrbitalState::PredictResult result = orbital_state.Predict(time);
  if (!result.Ok()) {
    return std::nullopt;
  }

  const TEME satellite_teme = result.GetValue();
  const ITRF satellite_itrf = ITRF::FromTEME(satellite_teme);

  const Horizontal horizontal =
      Horizontal::FromITRF(satellite_itrf, site_position);
  return horizontal.elevation;
}

// Calculate the satellite elevation at the median of two time points.
auto CalculateElevationAtMedian(const ITRF& site_position,
                                const OrbitalState& orbital_state,
                                const Time& time_a,
                                const Time& time_b) -> std::optional<double> {
  assert(time_a.GetScale() == time_b.GetScale());

  const JulianDate median_jd =
      (time_a.AsFormat<JulianDate>() + time_b.AsFormat<JulianDate>()) / 2;
  const Time median_time{median_jd, time_a.GetScale()};

  return CalculateElevationAtTime(site_position, orbital_state, median_time);
}

struct ApproximateAOSResult {
  // The satellite is visible at the start time point.
  bool is_visible_at_start_time{false};

  // The satellite is always visible: the satellite is visible at the start time
  // and there is no point in the past at which the satellite crossed the
  // horizon.
  bool is_always_visible{false};

  // The satellite is never above horizon throughout the prediction time window.
  bool is_never_visible{false};

  // Approximate AOS time, if it was found.
  //
  // Cases when the value is nullopt:
  //   - The satellite is never visible throughout the time window, in which
  //     case is_never_visible will be set to true.
  //   - The satellite is always visible, in which case is_always_visible will
  //     be set to true.
  std::optional<Time> time{};
};

// Get an approximate AOS when the satellite is above the horizon.
//
// If the satellite is visible at the start_time, then the AOS of its pass is
// looked backwards.
//
// The minimum elevation angle from the options is ignored because it is hard
// to guarantee the approximate step will sample the satellite trajectory close
// enough at its extremum, without requiring to do too many sampling points.
auto FindApproximateAOSAboveHorizon(const PredictPassOptions& options,
                                    const OrbitalState& orbital_state,
                                    const Time& start_time)
    -> ApproximateAOSResult {
  const int num_steps = GetNumPredictionSteps(
      TimeDifference::FromDays(options.num_days_to_predict),
      kApproximateTimeStep);

  bool is_visible_at_start_time = true;
  bool approximate_aos_time_found = false;

  Time approximate_aos_time = start_time;

  // Look forward in time for a moment when the satellite is above horizon.
  for (int i = 0; i < num_steps; ++i) {
    const std::optional<double> elevation = CalculateElevationAtTime(
        options.site_position, orbital_state, approximate_aos_time);
    if (!elevation) {
      return {};
    }

    if (*elevation > 0) {
      approximate_aos_time_found = true;
      break;
    }

    is_visible_at_start_time = false;
    approximate_aos_time += kApproximateTimeStep;
  }

  ApproximateAOSResult result;

  // If the satellite is never visible return an empty result.
  if (!approximate_aos_time_found) {
    result.is_never_visible = true;
    return result;
  }

  result.is_visible_at_start_time = is_visible_at_start_time;
  result.time = approximate_aos_time;

  // If the satellite was not visible at the starting time this means that the
  // approximate AOS time is somewhere in the future, and the actual AOS is
  // within kApproximateTimeStep in the past w.r.t to the approximate AOS time.
  if (!is_visible_at_start_time) {
    return result;
  }

  result.is_always_visible = true;

  // Look backwards for and AOS of the current pass.
  for (int i = 0; i < num_steps; ++i) {
    const Time previous_time = approximate_aos_time - kApproximateTimeStep;

    const std::optional<double> elevation = CalculateElevationAtTime(
        options.site_position, orbital_state, previous_time);
    if (!elevation) {
      return {};
    }

    if (*elevation < 0) {
      result.is_always_visible = false;
      result.time = approximate_aos_time;
      break;
    }

    approximate_aos_time = previous_time;
  }

  return result;
}

// Refine AOS by looking backward in time until the satellite moves below the
// horizon. Returns first time at which the satellite rise above the horizon.
//
// Assumes that the approximate AOS is within kApproximateTimeStep from the
// actual AOS.
auto RefineAOSAboveHorizon(const PredictPassOptions& options,
                           const OrbitalState& orbital_state,
                           const Time& approximate_aos_time) -> Time {
  Time refined_aos_time = approximate_aos_time;
  for (int i = 0; i < kRefineMaxSteps; ++i) {
    const Time previous_time = refined_aos_time - kRefineTimeStep;

    const std::optional<double> elevation = CalculateElevationAtTime(
        options.site_position, orbital_state, refined_aos_time);
    if (!elevation) {
      return {};
    }

    if (*elevation < 0) {
      break;
    }

    refined_aos_time = previous_time;
  }

  return refined_aos_time;
}

// Approximate LOS, so that the real LOS is within kApproximateTimeStep from the
// returned value. Looks forward the prediction period of time.
//
// If the satellite never goes below horizon throughout the prediction time
// window the nullopt is returned.
auto ApproximateLOSAboveHorizon(const PredictPassOptions& options,
                                const OrbitalState& orbital_state,
                                const astro_core::Time& start_time)
    -> std::optional<Time> {
  const int num_steps = GetNumPredictionSteps(
      TimeDifference::FromDays(options.num_days_to_predict),
      kApproximateTimeStep);

  Time approximate_los = start_time;
  for (int i = 0; i < num_steps; ++i) {
    const Time next_time = approximate_los + kApproximateTimeStep;

    const std::optional<double> elevation = CalculateElevationAtTime(
        options.site_position, orbital_state, next_time);
    if (!elevation) {
      return {};
    }

    if (*elevation < 0) {
      return approximate_los;
    }

    approximate_los = next_time;
  }

  return std::nullopt;
}

// Refine LOS by looking forward in time until the satellite moves below the
// horizon. Returns the last time at which the satellite is still above the
// horizon before going below it.
//
// Assumes that the approximate LOS is within kApproximateTimeStep from the
// actual LOS.
auto RefineLOSAboveHorizon(const PredictPassOptions& options,
                           const OrbitalState& orbital_state,
                           const astro_core::Time& approximate_los_time)
    -> Time {
  Time refined_los_time = approximate_los_time;
  for (int i = 0; i < kRefineMaxSteps; ++i) {
    const Time next_time = refined_los_time + kRefineTimeStep;

    const std::optional<double> elevation = CalculateElevationAtTime(
        options.site_position, orbital_state, next_time);
    if (!elevation) {
      // Return empty result if prediction has failed.
      return {};
    }

    if (*elevation < 0) {
      break;
    }

    refined_los_time = next_time;
  }

  return refined_los_time;
}

// Find the satellite LOS starting from the given moment in time.
//
// If the satellite never goes below the horizon throughout the prediction time
// window the nullopt is returned.
auto FindLOSAboveHorizon(const PredictPassOptions& options,
                         const OrbitalState& orbital_state,
                         const astro_core::Time& start_time)
    -> std::optional<Time> {
  const std::optional<Time> approximate_los_time =
      ApproximateLOSAboveHorizon(options, orbital_state, start_time);
  if (!approximate_los_time) {
    return std::nullopt;
  }

  return RefineLOSAboveHorizon(options, orbital_state, *approximate_los_time);
}

// Calculate the maximum satellite elevation during the given pass.
//
// If the satellite is not visible during the pass a value of 0 is returned.
//
// The start_time is the time from which the satellite pass prediction started.
auto CalculatePassMaxElevation(const PredictPassOptions& options,
                               const OrbitalState& orbital_state,
                               const SatellitePass& pass,
                               const Time& start_time) -> double {
  if (pass.is_never_visible) {
    return 0;
  }

  if (pass.aos && pass.los) {
    const std::optional<double> elevation = CalculateElevationAtMedian(
        options.site_position, orbital_state, *pass.aos, *pass.los);
    if (!elevation) {
      // Prediction failed, so can not get reliable elevation.
      return 0;
    }
    return *elevation;
  }

  double max_elevation = 0;
  Time time = pass.aos ? *pass.aos : start_time;

  const Time max_time =
      pass.los
          ? *pass.los
          : start_time + TimeDifference::FromDays(options.num_days_to_predict);
  const JulianDate max_jd = max_time.AsFormat<JulianDate>();

  for (;;) {
    const std::optional<double> elevation =
        CalculateElevationAtTime(options.site_position, orbital_state, time);
    if (!elevation) {
      return 0;
    }

    max_elevation = Max(max_elevation, *elevation);

    time += kApproximateTimeStep;
    if (time.AsFormat<JulianDate>() > max_jd) {
      break;
    }
  }

  return max_elevation;
}

// Get prediction of the currently visible pass, or the next visible pass.
//
// The satellite is considered visible when it is above the observer's horizon.
// The minimum elevation option is ignored.
//
// If the satellite is visible at the start_time then its AOS will be looked
// back, and LOS will be looked forward.
//
// If the satellite is not visible at the start_time a first AOS after the
// start_time is predicted (with its corresponding LOS).
auto PredictCurrentOrNextPassAboveHorizon(const PredictPassOptions& options,
                                          const OrbitalState& orbital_state,
                                          const Time& start_time)
    -> SatellitePass {
  const ApproximateAOSResult approximate_aos =
      FindApproximateAOSAboveHorizon(options, orbital_state, start_time);

  SatellitePass pass;

  if (approximate_aos.is_never_visible) {
    // No AOS detected throughout the prediction time period, so return an empty
    // pass.
    pass.is_never_visible = true;
    return pass;
  }

  // Refine AOS if there was detected transition of the satellite to ever leave
  // the horizon.
  if (!approximate_aos.is_always_visible) {
    pass.aos =
        RefineAOSAboveHorizon(options, orbital_state, *approximate_aos.time);
  }

  // If the satellite is visible at the start time then start looking for LOS
  // starting from the given start time. This avoids duplicate elevation
  // calculation: the AOS might've looked in the past calculating elevation.
  const Time los_start_time = approximate_aos.is_visible_at_start_time
                                  ? start_time
                                  : *approximate_aos.time;

  pass.los = FindLOSAboveHorizon(options, orbital_state, los_start_time);

  // The satellite never went below the horizon throughout the prediction time
  // window.
  if (!pass.los && approximate_aos.is_always_visible) {
    pass.is_always_visible = true;
  }

  pass.max_elevation =
      CalculatePassMaxElevation(options, orbital_state, pass, start_time);

  return pass;
}

}  // namespace

auto PredictCurrentOrNextPass(const PredictPassOptions& options,
                              const OrbitalState& orbital_state,
                              const Time& start_time) -> SatellitePass {
  const JulianDate max_jd =
      start_time.AsFormat<JulianDate>() + options.num_days_to_predict;

  Time pass_start_time = start_time;
  for (;;) {
    const SatellitePass pass = PredictCurrentOrNextPassAboveHorizon(
        options, orbital_state, pass_start_time);

    if (pass.is_never_visible) {
      return pass;
    }

    if (pass.max_elevation >= options.min_elevation) {
      return pass;
    }

    if (pass.is_always_visible) {
      // The satellite is always visible throughout the prediction time window,
      // it will not move above its currently calculated maximum elevation.
      return SatellitePass{.is_never_visible = true};
    }

    assert(pass.los);

    pass_start_time = *pass.los + kApproximateTimeStep;

    if (pass_start_time.AsFormat<JulianDate>() > max_jd) {
      break;
    }
  }

  return {};
}

auto PredictNextPass(const PredictPassOptions& options,
                     const OrbitalState& orbital_state,
                     const Time& start_time) -> SatellitePass {
  const std::optional<double> current_elevation = CalculateElevationAtTime(
      options.site_position, orbital_state, start_time);
  if (!current_elevation) {
    // Satellite trajectory calculation has failed.
    return {};
  }

  Time next_time = start_time;

  if (*current_elevation > 0) {
    std::optional<Time> los_time =
        FindLOSAboveHorizon(options, orbital_state, next_time);
    if (!los_time) {
      if (*current_elevation < options.min_elevation) {
        return SatellitePass{
            .is_never_visible = true,
        };
      }

      SatellitePass pass = {
          .is_always_visible = true,
      };
      pass.max_elevation =
          CalculatePassMaxElevation(options, orbital_state, pass, start_time);
      return pass;
    }

    next_time = *los_time + kApproximateTimeStep;
  }

  return PredictCurrentOrNextPass(options, orbital_state, next_time);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
