// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Higher-level conversion utilities between different frames.

#pragma once

#include "astro_core/numeric/numeric.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Time;

// Conversion matrix for TEME-to-PEF conversion:
//   r_pef = TEMEToPEFMatrix(time) * r_teme
auto TEMEToPEFMatrix(const Time& time) -> Mat3;

// Conversion matrix for PEF-to-ITRF conversion:
//   r_itrf = PEFToITRFMatrix(time) * r_pef
auto PEFToITRFMatrix(const Time& time) -> Mat3;

// Conversion matrix for TEME-to-ITRF conversion:
//   r_itrf = TEMEToITRFMatrix(time) * r_teme
auto TEMEToITRFMatrix(const Time& time) -> Mat3;

// Convert position and velocity from TEME to ITRF.
// This is more optimal than converting the position of velocity separately
// using individually constructed matrices.
void TEMEToITRF(const Time& time,
                const Vec3& r_teme,
                const Vec3& v_teme,
                Vec3& r_itrf,
                Vec3& v_itrf);

// Convert position and velocity from ITRF to TEME.
// This is more optimal than converting the position of velocity separately
// using individually constructed matrices.
void ITRFToTEME(const Time& time,
                const Vec3& r_itrf,
                const Vec3& v_itrf,
                Vec3& r_teme,
                Vec3& v_teme);

// Convert position and velocity from ITRF to GCRF.
// Operates in cartesian representation for both ITRF and GCRF.
void ITRFToGCRF(const Time& time,
                const Vec3& r_itrf,
                const Vec3& v_itrf,
                Vec3& r_gcrf,
                Vec3& v_gcrf);

// Convert position and velocity from GCRF to ITRF.
// Operates in cartesian representation for both ITRF and GCRF.
void GCRFToITRF(const Time& time,
                const Vec3& r_itrf,
                const Vec3& v_itrf,
                Vec3& r_gcrf,
                Vec3& v_gcrf);

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
