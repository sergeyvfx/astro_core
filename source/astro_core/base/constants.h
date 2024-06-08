// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include <numbers>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace constants {

// Templated constants from the std::numbers namespace.
template <class T>
inline constexpr T e_v = std::numbers::e_v<T>;
template <class T>
inline constexpr T log2e_v = std::numbers::log2e_v<T>;
template <class T>
inline constexpr T log10e_v = std::numbers::log10e_v<T>;
template <class T>
inline constexpr T pi_v = std::numbers::pi_v<T>;
template <class T>
inline constexpr T inv_pi_v = std::numbers::inv_pi_v<T>;
template <class T>
inline constexpr T inv_sqrtpi_v = std::numbers::inv_sqrtpi_v<T>;
template <class T>
inline constexpr T ln2_v = std::numbers::ln2_v<T>;
template <class T>
inline constexpr T ln10_v = std::numbers::ln10_v<T>;
template <class T>
inline constexpr T sqrt2_v = std::numbers::sqrt2_v<T>;
template <class T>
inline constexpr T sqrt3_v = std::numbers::sqrt3_v<T>;
template <class T>
inline constexpr T inv_sqrt3_v = std::numbers::inv_sqrt3_v<T>;
template <class T>
inline constexpr T egamma_v = std::numbers::egamma_v<T>;
template <class T>
inline constexpr T phi_v = std::numbers::phi_v<T>;

// Constants from the std::numbers namespace.
inline constexpr double e = std::numbers::e;
inline constexpr double log2e = std::numbers::log2e;
inline constexpr double log10e = std::numbers::log10e;
inline constexpr double pi = std::numbers::pi;
inline constexpr double inv_pi = std::numbers::inv_pi;
inline constexpr double inv_sqrtpi = std::numbers::inv_sqrtpi;
inline constexpr double ln2 = std::numbers::ln2;
inline constexpr double ln10 = std::numbers::ln10;
inline constexpr double sqrt2 = std::numbers::sqrt2;
inline constexpr double sqrt3 = std::numbers::sqrt3;
inline constexpr double inv_sqrt3 = std::numbers::inv_sqrt3;
inline constexpr double egamma = std::numbers::egamma;
inline constexpr double phi = std::numbers::phi;

inline constexpr double kNumMinutesInDay = 1440.0;
inline constexpr double kNumSecondsInDay = 86400.0;
inline constexpr double kNumSecondsInHour = 3600.0;

// Epoch of the Unix epoch (January 1, 1970) in Julian date.
inline constexpr double kJulianDateEpochUnixEpoch = 2440587.5;

// Epoch of MJD in Julain Date.
//   MJD = JD - kJulianDateEpochMJD
inline constexpr double kJulianDateEpochMJD = 2400000.5;

// Epoch of J2000 in Julian Date.
inline constexpr double kJulianDateEpochJ2000 = 2451545.0;

// The number of days in the Julian year and century.
inline constexpr double kNumDaysInJulianYear = 365.25;
inline constexpr double kNumDaysInJulianCentury = 36525;

// The speed of light, in vacuum, meters per second.
inline constexpr double kSpeedOfLight = 299792458;

// Astronomical unit, meters.
inline constexpr double kAstronomicalUnit = 149597870700;

}  // namespace constants
}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
