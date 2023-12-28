// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Time scale, also referred to as time standard.
// This is a specification for measuring time: the rate at which time passes.

#pragma once

#include <ostream>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

enum class TimeScale {
  // International Atomic Time (TAI).
  //
  // It is a continuous scale of time, without leap seconds, and it is the
  // principal realisation of Terrestrial Time (TT).
  kTAI,

  // Coordinated Universal Time scale (UTC).
  //
  // An approximation of the Universal Time, and differs from TAI by an integral
  // number of seconds (for dates after January 1, 1972).
  kUTC,

  // Universal Time (UT or UT1).
  //
  // Is a time standard based on Earth's rotation which is computed from a
  // measure of the Earth's angle with respect to the International Celestial
  // Reference Frame (ICRF), called the Earth Rotation Angle (ERA, which serves
  // as a modern replacement for Greenwich Mean Sidereal Time). UT1 is the same
  // everywhere on Earth.
  //
  // Reference:
  //   https://en.wikipedia.org/wiki/Universal_Time
  kUT1,

  // Terrestrial Time (TT) using TAI realization.
  //
  // The unit of TT is the SI second, the definition of which is based currently
  // on the caesium atomic clock,[3] but TT is not itself defined by atomic
  // clocks. It is a theoretical ideal, and real clocks can only approximate it.
  //
  // The main realization of TT is supplied by TAI which is defined as:
  //
  //   TT(TAI) = TAI + 32.184s
  //
  // Reference:
  //   https://en.wikipedia.org/wiki/Terrestrial_Time
  kTT,

  // TODO(sergey): Support kTDB time scale.
  //
  // Reference:
  //   https://en.wikipedia.org/wiki/Barycentric_Dynamical_Time
  //   https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems#TAI_-_TDT,_TCG,_TT
};

constexpr auto TimeScaleAsString(const TimeScale scale) -> const char* {
  switch (scale) {
    case TimeScale::kTAI: return "TAI";
    case TimeScale::kUTC: return "UTC";
    case TimeScale::kUT1: return "UT1";
    case TimeScale::kTT: return "TT";
  }
  return "UNKNOWN";
}

inline auto operator<<(std::ostream& os, const TimeScale scale)
    -> std::ostream& {
  os << TimeScaleAsString(scale);
  return os;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
