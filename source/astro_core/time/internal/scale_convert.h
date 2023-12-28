// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Conversion utilities between time scales.
//
// Time scale conversion between two time scales happens via TAI scale. This
// means that, for example, UT1 is converted to UTC in two steps: UT1 to TAI,
// and TAI to UTC.

#pragma once

#include <cassert>

#include "astro_core/base/constants.h"
#include "astro_core/base/unreachable.h"
#include "astro_core/earth/leap_second.h"
#include "astro_core/earth/orientation.h"
#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/time/scale.h"
#include "astro_core/time/time.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace time_internal {

////////////////////////////////////////////////////////////////////////////////
// Partial definitions of code-generators.

// The ToTAI<scale>::Convert(time) converts time given time from kFromScale
// to TAI. It is expected that the time has the matches scale.
template <TimeScale kFromScale>
struct ToTAI;

// The FromTAI<scale>::Convert(time) converts time from TAI time scale to the
// kToScale. It is expected that the time has the TAI time scale.
template <TimeScale kToScale>
struct FromTAI;

// A helper function which runs a conversion method of a specialized converter.
// Basically it calls `Converter<scale>::Convert(time, args...)` but with
// `scale` being a variable.
template <template <TimeScale> class Converter, class... Args>
inline auto RunConvert(const TimeScale scale, const Time& time, Args... args)
    -> Time {
  switch (scale) {
    case TimeScale::kTAI:
      return Converter<TimeScale::kTAI>::Convert(time, args...);
    case TimeScale::kUTC:
      return Converter<TimeScale::kUTC>::Convert(time, args...);
    case TimeScale::kUT1:
      return Converter<TimeScale::kUT1>::Convert(time, args...);
    case TimeScale::kTT:
      return Converter<TimeScale::kTT>::Convert(time, args...);
  }
  UNREACHABLE();
}

// Specialization of time scale conversion from kFromScale to kToScale.
// The default implementation converts the time to TAI, and then to the
// kToScale.
//
// It is possible to override conversion process for a specific time scales
// by providing a specialization of the ScaleConverter class. For example:
//
//   template <TimeScale::kUT1, TimeScale::kUTC>
//   struct ScaleConverter {
//     inline static auto Convert(const Time& time) -> Time {
//       ...
//     }
//   }
template <TimeScale kFromScale, TimeScale kToScale>
struct ScaleConverter {
  inline static auto Convert(const Time& time) -> Time {
    const Time time_tai = RunConvert<ToTAI>(time.GetScale(), time);
    assert(time_tai.GetScale() == TimeScale::kTAI);

    const Time result = RunConvert<FromTAI>(kToScale, time_tai);
    assert(result.GetScale() == kToScale);
    return result;
  }
};

// Helper class which converts source and destination time scales from variables
// to template arguments.
//
// The typical usage is:
//   RunConvert<ScaleConverterChooser>(time.GetScale(), time, to_scale);
//
// Doing so will end up in a call similar to
//   ScaleConverter<time.scale, to_scale>::Convert(time);
template <TimeScale kFromScale>
struct ScaleConverterChooser {
  static auto Convert(const Time& time, const TimeScale to_scale) -> Time {
    return RunConvert<Converter>(to_scale, time);
  }

  template <TimeScale kToScale>
  struct Converter {
    static auto Convert(const Time& time) -> Time {
      return ScaleConverter<kFromScale, kToScale>::Convert(time);
    }
  };
};

////////////////////////////////////////////////////////////////////////////////
// Entry point of time scale conversion.

// Convert the time time its scale to the specified scale.
inline auto ConvertToScale(const Time& time, const TimeScale to_scale) -> Time {
  return RunConvert<ScaleConverterChooser>(time.GetScale(), time, to_scale);
}

////////////////////////////////////////////////////////////////////////////////
// TAI time scale.

template <>
struct ToTAI<TimeScale::kTAI> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kTAI);
    return time;
  }
};

template <>
struct FromTAI<TimeScale::kTAI> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kTAI);
    return time;
  }
};

////////////////////////////////////////////////////////////////////////////////
// UTC time scale.

template <>
struct ToTAI<TimeScale::kUTC> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kUTC);

    const ModifiedJulianDate mjd_utc = time.AsFormat<ModifiedJulianDate>();

    const double tai_minus_utc = GetTAIMinusUTCSecondsInUTCScale(mjd_utc);

    const ModifiedJulianDate tai_mjd(DoubleDouble(mjd_utc) +
                                     DoubleDouble(tai_minus_utc) /
                                         constants::kNumSecondsInDay);
    return Time(tai_mjd, TimeScale::kTAI);
  }
};

template <>
struct FromTAI<TimeScale::kUTC> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kTAI);

    const ModifiedJulianDate tai_mjd = time.AsFormat<ModifiedJulianDate>();

    const double tai_minus_utc = GetTAIMinusUTCSecondsInTAIScale(tai_mjd);

    const ModifiedJulianDate mjd_utc(DoubleDouble(tai_mjd) -
                                     DoubleDouble(tai_minus_utc) /
                                         constants::kNumSecondsInDay);
    return Time(mjd_utc, TimeScale::kUTC);
  }
};

////////////////////////////////////////////////////////////////////////////////
// UT1 time scale.

template <>
struct ToTAI<TimeScale::kUT1> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kUT1);

    // Start with estimating the UTC using UT1 to get an initial guess of the
    // UT1-UTC correction. Then correct the UT1 guess with with UT1-UTC and
    // repeat the process. This allows to more accurately estimate time at
    // around leap second.
    //
    // This is similar to algorithm used in Astropy. The difference there is
    // that the estimate around the leap second is corrected using ERFA. The
    // algorithm used in ERFA is not really clear.
    //
    // It is also similar to the Skyfield. The difference there is that TT is
    // used there instead of UTC.

    ModifiedJulianDate utc_mjd_approx;
    DoubleDouble ut1_minus_utc_sec_approx;

    utc_mjd_approx = time.AsFormat<ModifiedJulianDate>();
    ut1_minus_utc_sec_approx = GetUT1MinusUTCSecondsInUTCScale(utc_mjd_approx);

    utc_mjd_approx =
        utc_mjd_approx - ut1_minus_utc_sec_approx / constants::kNumSecondsInDay;
    ut1_minus_utc_sec_approx = GetUT1MinusUTCSecondsInUTCScale(utc_mjd_approx);

    utc_mjd_approx = time.AsFormat<ModifiedJulianDate>() -
                     ut1_minus_utc_sec_approx / constants::kNumSecondsInDay;

    return ToTAI<TimeScale::kUTC>::Convert(
        Time(utc_mjd_approx, TimeScale::kUTC));
  }
};

template <>
struct FromTAI<TimeScale::kUT1> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kTAI);

    const ModifiedJulianDate tai_mjd = time.AsFormat<ModifiedJulianDate>();

    // Convert the input time to UTC and look up the Earth orientation
    // parameters.

    const Time time_utc = FromTAI<TimeScale::kUTC>::Convert(time);
    const ModifiedJulianDate utc_mjd{time_utc.AsFormat<ModifiedJulianDate>()};

    const DoubleDouble ut1_minus_utc_sec =
        GetUT1MinusUTCSecondsInUTCScale(utc_mjd);
    const DoubleDouble ut1_minus_utc_jd =
        ut1_minus_utc_sec / constants::kNumSecondsInDay;

    // TAI-UTC.
    //
    // Calculate as the difference between input TAI time and its UTC
    // complementary value. The difference is in Julian days.
    const DoubleDouble tai_minus_utc_jd{tai_mjd - utc_mjd};

    // Calculate the UT1-TAI from UT1-UTC and TAI-UTC:
    // (UT1-UTC) - (TAI-UTC) = UT1 - UTC - TAI + UTC = UT1-TAI
    const DoubleDouble ut1_minus_tai_jd = ut1_minus_utc_jd - tai_minus_utc_jd;

    // UT1 = TAI + UT1-TAI
    const DoubleDouble ut1_mjd{tai_mjd + ut1_minus_tai_jd};
    return Time(ModifiedJulianDate(ut1_mjd), TimeScale::kUT1);
  }
};

////////////////////////////////////////////////////////////////////////////////
// TT(TAI0 time scale.

template <>
struct ToTAI<TimeScale::kTT> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kTT);

    const ModifiedJulianDate tt_mjd = time.AsFormat<ModifiedJulianDate>();

    const ModifiedJulianDate tai_mjd(DoubleDouble(tt_mjd) -
                                     DoubleDouble(32.184) /
                                         constants::kNumSecondsInDay);

    return Time(tai_mjd, TimeScale::kTAI);
  }
};

template <>
struct FromTAI<TimeScale::kTT> {
  static auto Convert(const Time& time) -> Time {
    assert(time.GetScale() == TimeScale::kTAI);

    const ModifiedJulianDate tai_mjd = time.AsFormat<ModifiedJulianDate>();

    const ModifiedJulianDate tt_mjd(DoubleDouble(tai_mjd) +
                                    DoubleDouble(32.184) /
                                        constants::kNumSecondsInDay);

    return Time(tt_mjd, TimeScale::kTT);
  }
};

}  // namespace time_internal
}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
