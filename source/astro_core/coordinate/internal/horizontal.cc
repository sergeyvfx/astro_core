
// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/horizontal.h"

#include <limits>

#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/itrf.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/numeric.h"

// References:
//
//   [Vallado2006] Vallado, David A., Paul Crawford, Richard Hujsak, and T.S.
//       Kelso, "Revisiting Spacetrack Report #3," presented at the AIAA/AAS
//       Astrodynamics Specialist Conference, Keystone, CO, 2006 August 21–24.

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto Horizontal::FromITRF(const ITRF& itrf,
                          const ITRF& site_itrf) -> Horizontal {
  // [Vallado2006] ALGORITHM 27: RAZEL

  const Geodetic gd = Geodetic::FromITRF(site_itrf);
  const double phi_gd = gd.latitude;
  const double lambda = gd.longitude;

  const Vec3 r_ecef = itrf.position.GetCartesian();
  const Vec3 v_ecef = itrf.velocity.GetCartesianOr({0, 0, 0});

  const Vec3 r_site_ecef = site_itrf.position.GetCartesian();

  const Vec3 rho_ecef = r_ecef - r_site_ecef;
  const Vec3 drho_ecef = v_ecef;

  const Mat3 ecef_to_sez = ROT2(constants::pi / 2 - phi_gd) * ROT3(lambda);
  const Vec3 rho_sez = ecef_to_sez * rho_ecef;
  const Vec3 drho_sez = ecef_to_sez * drho_ecef;

  const double rho = rho_sez.Norm();

  const double denom = Sqrt(rho_sez(0) * rho_sez(0) + rho_sez(1) * rho_sez(1));

  double elevation, azimuth;

  if (denom < std::numeric_limits<double>::epsilon()) {
    elevation = Sign(rho_sez(2)) * (constants::pi / 2);
    azimuth = ArcTan2(drho_sez(1), -drho_sez(0));
  } else {
    elevation = ArcSin(rho_sez(2) / rho);
    azimuth = ArcTan2(rho_sez(1) / denom, -rho_sez(0) / denom);
  }

  if (azimuth < 0) {
    azimuth += constants::pi * 2;
  }

  return Horizontal({
      .observation_time = itrf.observation_time,
      .elevation = elevation,
      .azimuth = azimuth,
      .distance = rho,
  });
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
