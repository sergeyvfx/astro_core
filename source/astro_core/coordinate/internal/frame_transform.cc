// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// References:
//
//   [Vallado2006] Vallado, David A., Paul Crawford, Richard Hujsak, and T.S.
//       Kelso, "Revisiting Spacetrack Report #3," presented at the AIAA/AAS
//       Astrodynamics Specialist Conference, Keystone, CO, 2006 August 21–24.
//
//   [Vallado2013] David A Vallado and Wayne D Macclain. 2013.
//       Fundamentals of astrodynamics and applications.
//
//   [satstuff] Compute SGP4 Satellite Propagator Output in GCRS with astropy
//       https://github.com/egemenimre/satstuff/blob/master/notebooks/astropy/sgp4_teme_astropy.ipynb
//
//   [IERS2010] Gerard Petit, and Brian Luzum, IERS Conventions (2010).

#include "astro_core/coordinate/frame_transform.h"

#include "astro_core/earth/celestial_intermediate_pole.h"
#include "astro_core/earth/earth.h"
#include "astro_core/earth/orientation.h"
#include "astro_core/earth/rotation.h"
#include "astro_core/earth/terrestrial_intermediate_origin.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/format/modified_julian_date.h"
#include "astro_core/time/greenwich_sidereal_time.h"
#include "astro_core/time/time.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto TEMEToPEFMatrix(const Time& time) -> Mat3 {
  // Follows the following:
  //   [Vallado2013] Eq. (3-90).
  //   [Vallado2006] Eq. (1).
  //
  // A bit confusing part here is that in the [Vallado2006] Eq. (C-2) the matrix
  // is transposed, but it is not in other implementations like Astropy.

  const JulianDate ut1_jd =
      time.ToScale<TimeScale::kUT1>().AsFormat<JulianDate>();

  const double gmst82{GreenwichMeanSiderealTime1982(ut1_jd)};

  return ROT3(gmst82);
}

auto PEFToITRFMatrix(const Time& time) -> Mat3 {
  const ModifiedJulianDate utc_mjd =
      time.ToScale<TimeScale::kUTC>().AsFormat<ModifiedJulianDate>();

  // TODO(sergey): Investigate storing radians in the Earth orientation table.
  const Vec2 polar_motion = GetEarthPolarMotionInUTCScale(utc_mjd);

  // [Vallado2013] Eq. (3-77).
  // [Vallado2006] Eq. (C-2).
  //
  // [ROT2(xp)*ROT1(yp)]_T = [ROT1(-yp)*ROT2(-xp)]
  return ROT1(-polar_motion(1)) * ROT2(-polar_motion(0));
}

auto TEMEToITRFMatrix(const Time& time) -> Mat3 {
  const Mat3 teme_to_pef = TEMEToPEFMatrix(time);
  const Mat3 pef_to_itrf = PEFToITRFMatrix(time);

  return pef_to_itrf * teme_to_pef;
}

void TEMEToITRF(const Time& time,
                const Vec3& r_teme,
                const Vec3& v_teme,
                Vec3& r_itrf,
                Vec3& v_itrf) {
  constexpr Vec3 omega{0.0, 0.0, Earth::kOmega};

  const Mat3 teme_to_pef = TEMEToPEFMatrix(time);
  const Mat3 pef_to_itrf = PEFToITRFMatrix(time);

  const Vec3 r_pef = teme_to_pef * r_teme;

  r_itrf = pef_to_itrf * r_pef;

  // Inspiration from [satstuff] and [Vallado2013] Eq. (3-80).
  // The former one seems to be matching more closely here.
  //
  // TODO(sergey): Verify the math, as there seems to be no exact formula
  // provided specifically for v_teme -> v_itrf. It is very similar to the
  // v_itrf to v_gcrf conversion on [Vallado2013] Page 220.
  //
  // It is possible to increase precision of the Earth omega using [Vallado2013]
  // Eq. (3-40) but the difference is probably below of what the rest of the
  // calculations here can achieve anyway.
  v_itrf = pef_to_itrf * (teme_to_pef * v_teme - omega.Cross(r_pef));
}

void ITRFToTEME(const Time& time,
                const Vec3& r_itrf,
                const Vec3& v_itrf,
                Vec3& r_teme,
                Vec3& v_teme) {
  constexpr Vec3 omega{0.0, 0.0, Earth::kOmega};

  const Mat3 pef_to_teme = TEMEToPEFMatrix(time).Transposed();
  const Mat3 itrf_to_pef = PEFToITRFMatrix(time).Transposed();

  const Vec3 r_pef = itrf_to_pef * r_itrf;

  r_teme = pef_to_teme * r_pef;
  v_teme = pef_to_teme * (itrf_to_pef * v_itrf + omega.Cross(r_pef));
}

void ITRFToGCRF(const Time& time,
                const Vec3& r_itrf,
                const Vec3& v_itrf,
                Vec3& r_gcrf,
                Vec3& v_gcrf) {
  // Implements Method (1) from [IERS2010] Section 5.9, Page 69.
  // This method is also described in [Vallado2013] Page 220.

  constexpr Vec3 omega{0.0, 0.0, Earth::kOmega};

  const JulianDate jd_tt =
      time.ToScale<TimeScale::kTT>().AsFormat<JulianDate>();
  const JulianDate jd_ut1 =
      time.ToScale<TimeScale::kUT1>().AsFormat<JulianDate>();
  const ModifiedJulianDate mjd_utc =
      time.ToScale<TimeScale::kUTC>().AsFormat<ModifiedJulianDate>();

  const Vec2 cip_xy = CelestialIntermediatePole(jd_tt);
  const Vec2 polar_motion = GetEarthPolarMotionInUTCScale(mjd_utc);
  const double s = CelestialIntermediateOriginLocator(jd_tt, cip_xy);
  const double s_prime = TerrestrialIntermediateOriginLocator(jd_tt);
  const double era = EarthRotationAngle(jd_ut1);

  const Mat3 gcrf_to_cirs = CelestialToIntermediateFrameOfDateMatrix(cip_xy, s);
  const Mat3 cirs_to_tirs = AxisRotationAroundZ(era);
  const Mat3 tirs_to_itrf = ROT1(-polar_motion(1)) * ROT2(-polar_motion(0)) *
                            AxisRotationAroundZ(s_prime);

  const Mat3 gcrf_to_tirs = cirs_to_tirs * gcrf_to_cirs;
  const Mat3 gcrf_to_itrf = tirs_to_itrf * gcrf_to_tirs;

  const Mat3 itrf_to_gcrf = gcrf_to_itrf.Transposed();

  r_gcrf = itrf_to_gcrf * r_itrf;

  // Perform transformation of velocity.
  // Follows equations from [Vallado2013] Page 220.

  // Inverse the individual matrices and give them the same naming as in the
  // [Vallado2013].
  const Mat3 PN = gcrf_to_cirs.Transposed();
  const Mat3 R = cirs_to_tirs.Transposed();
  const Mat3 W = tirs_to_itrf.Transposed();

  const Vec3 r_tirs = W * r_itrf;

  v_gcrf = PN * R * (W * v_itrf + omega.Cross(r_tirs));
}

void GCRFToITRF(const Time& time,
                const Vec3& r_gcrf,
                const Vec3& v_gcrf,
                Vec3& r_itrf,
                Vec3& v_itrf) {
  // The implementation is the reverse of ITRFToGCRF.

  constexpr Vec3 omega{0.0, 0.0, Earth::kOmega};

  const JulianDate jd_tt =
      time.ToScale<TimeScale::kTT>().AsFormat<JulianDate>();
  const JulianDate jd_ut1 =
      time.ToScale<TimeScale::kUT1>().AsFormat<JulianDate>();
  const ModifiedJulianDate mjd_utc =
      time.ToScale<TimeScale::kUTC>().AsFormat<ModifiedJulianDate>();

  const Vec2 cip_xy = CelestialIntermediatePole(jd_tt);
  const Vec2 polar_motion = GetEarthPolarMotionInUTCScale(mjd_utc);
  const double s = CelestialIntermediateOriginLocator(jd_tt, cip_xy);
  const double s_prime = TerrestrialIntermediateOriginLocator(jd_tt);
  const double era = EarthRotationAngle(jd_ut1);

  const Mat3 gcrf_to_cirs = CelestialToIntermediateFrameOfDateMatrix(cip_xy, s);
  const Mat3 cirs_to_tirs = AxisRotationAroundZ(era);
  const Mat3 tirs_to_itrf = ROT1(-polar_motion(1)) * ROT2(-polar_motion(0)) *
                            AxisRotationAroundZ(s_prime);

  const Mat3 gcrf_to_tirs = cirs_to_tirs * gcrf_to_cirs;
  const Mat3 gcrf_to_itrf = tirs_to_itrf * gcrf_to_tirs;

  r_itrf = gcrf_to_itrf * r_gcrf;

  // Perform transformation of velocity.
  // Follows equations from [Vallado2013] Page 220.

  // Convert naming to the one used in [Vallado2013] for simplicity.
  const Mat3& PN_prime = gcrf_to_cirs;
  const Mat3& R_prime = cirs_to_tirs;
  const Mat3& W_prime = tirs_to_itrf;

  const Vec3 r_tirs = tirs_to_itrf.Transposed() * r_itrf;

  v_itrf = W_prime * (R_prime * PN_prime * v_gcrf - omega.Cross(r_tirs));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
