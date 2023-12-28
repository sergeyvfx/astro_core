// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/orbital_state.h"

#include "astro_core/base/constants.h"
#include "astro_core/math/math.h"
#include "astro_core/satellite/tle.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/time.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

namespace SGP4Funcs = sgp_internal::SGP4Funcs;

// JD date of the SGP4 epoch (1949 December 31 00:00 UT).
constexpr DoubleDouble kSGP4EpochInJD{2433281.0, 0.5};

// Parameters in the naming and units expected by the SGP4 library.
//
// These parameters are needed to fully initialize `elsetrec` structure. This
// includes parameters which are needed for `sgp4init()`, as well as for the
// direct initialization of the `elsetrec` structure whuch mimics the behavior
// of the `twoline2rv`.
struct SGP4Parameters {
  DoubleDouble epoch_jd;

  double epoch;     // Days since 1949 December 31 00:00 UT
  double bstar;     // Drag coefficient (1/earth radii)
  double ndot;      // Ballistic coefficient (radians/minute^2)
  double nddot;     // Mean motion 2nd derivative (radians/minute^3)
  double ecco;      // Eccentricity.
  double argpo;     // Argument of perigee (radians).
  double inclo;     // Inclination (radians).
  double mo;        // Mean anomaly (radians).
  double no_kozai;  // Mean motion (radians/minute)
  double nodeo;     // Right ascension of ascending node (radians)
};

// Return TLE epoch represented in JD format.
constexpr auto GetTLEEpochInJD(const TLE& tle) -> DoubleDouble {
  const Time epoch(tle.epoch, TimeScale::kUTC);
  return DoubleDouble(epoch.AsFormat<JulianDate>());
}

// Get parameters for the SGP4 model from the TLE.
constexpr auto GetSGP4Parameters(const TLE& tle) -> SGP4Parameters {
  SGP4Parameters parameters;

  parameters.epoch_jd = GetTLEEpochInJD(tle);

  parameters.epoch = double(parameters.epoch_jd - kSGP4EpochInJD);
  parameters.bstar = tle.b_star;
  parameters.ndot =
      RevolutionsPerDayToRadiansPerMinute(tle.mean_motion_first_derivative) /
      constants::kNumMinutesInDay;
  parameters.nddot =
      RevolutionsPerDayToRadiansPerMinute(tle.mean_motion_second_derivative) /
      (constants::kNumMinutesInDay * constants::kNumMinutesInDay);
  parameters.ecco = tle.eccentricity;
  parameters.argpo = DegreesToRadians(tle.argument_of_perigee);
  parameters.inclo = DegreesToRadians(tle.inclination);
  parameters.mo = DegreesToRadians(tle.mean_anomaly);
  parameters.no_kozai = RevolutionsPerDayToRadiansPerMinute(tle.mean_motion);
  parameters.nodeo = DegreesToRadians(tle.raan);

  return parameters;
}

}  // namespace

auto OrbitalState::InitializeFromTLE(const TLE& tle) -> bool {
  const SGP4Parameters parameters = GetSGP4Parameters(tle);

  // Initialize different representations of the epoch.
  // This mimics behavior of the `twoline2rv()` since these fields are not
  // taken care of by the `sgp4init()`.
  sgp4_satrec_.jdsatepoch = parameters.epoch_jd.GetHi();
  sgp4_satrec_.jdsatepochF = parameters.epoch_jd.GetLo();
  sgp4_satrec_.epochyr = tle.epoch.GetYear() % 100;
  sgp4_satrec_.epochdays = tle.epoch.GetDecimalDay();

  return SGP4Funcs::sgp4init(sgp_internal::wgs72,
                             'i',
                             "",
                             parameters.epoch,
                             parameters.bstar,
                             parameters.ndot,
                             parameters.nddot,
                             parameters.ecco,
                             parameters.argpo,
                             parameters.inclo,
                             parameters.mo,
                             parameters.no_kozai,
                             parameters.nodeo,
                             sgp4_satrec_);
}

namespace {

inline auto TranslateError(const sgp_internal::elsetrec& sgp4_satrec)
    -> OrbitalState::Error {
  switch (sgp4_satrec.error) {
    case 1: return OrbitalState::Error::kMeanElementsRange;
    case 2: return OrbitalState::Error::kMeanMotionRange;
    case 3: return OrbitalState::Error::kPortElementsRange;
    case 4: return OrbitalState::Error::kSemiLatusRectumRange;
    case 5: return OrbitalState::Error::kSuborbitalEpochElements;
    case 6: return OrbitalState::Error::kSatelliteDecayed;
  }

  return OrbitalState::Error::kError;
}

}  // namespace

auto OrbitalState::Predict(const Time& time) const -> PredictResult {
  const DoubleDouble jd{time.ToScale<TimeScale::kUTC>().AsFormat<JulianDate>()};

  const DoubleDouble jd_epoch{sgp4_satrec_.jdsatepoch,
                              sgp4_satrec_.jdsatepochF};
  const DoubleDouble time_since_epoch_min =
      (jd - jd_epoch) * constants::kNumMinutesInDay;

  // Make a copy to allow use from multiple threads.
  // This is needed because the `SGP4Funcs::sgp4()` modifies the elsetrec struct
  // passed to it.
  //
  // TODO(sergey): Investigate whether there is a performance impact of doing so
  // for consecutive prediction requests.
  sgp_internal::elsetrec local_sgp4_satrec = sgp4_satrec_;

  Vec3 position, velocity;
  if (!SGP4Funcs::sgp4(local_sgp4_satrec,
                       double(time_since_epoch_min),
                       position.Pointer(),
                       velocity.Pointer())) {
    return PredictResult{TranslateError(local_sgp4_satrec)};
  }

  // Convert kilometers provided by the SGP4 to meters which is the expected
  // units in the API.
  return PredictResult{TEME{{.observation_time = time,
                             .position = position * 1000.0,
                             .velocity = velocity * 1000.0}}};
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
