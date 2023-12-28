// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Traits of the time format.
//
// Defines functions needed to convert the time representation to and from the
// format used internally in the module. The internal representation is the
// Julian date format, as it allows to more easily represent date in different
// time scales.
//
// The format traits is to implement the following functions:
//
//   static auto ToJulianDate(const MyFormat& time) -> DoubleDouble;
//   static auto FromJulianDate(const DoubleDouble& jd) -> MyFormat;
//
// A bare DoubleDouble scalar is used instead of something higher level like
// JulianDate to avoid circular dependencies, and avoid pulling extra headers.
template <class Format>
struct TimeFormatTraits;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
