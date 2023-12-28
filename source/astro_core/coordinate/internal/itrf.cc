// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/itrf.h"

#include "astro_core/coordinate/frame_transform.h"
#include "astro_core/coordinate/gcrf.h"
#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/teme.h"
#include "astro_core/earth/earth.h"
#include "astro_core/math/math.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto ITRF::FromGCRF(const GCRF& gcrf) -> ITRF {
  Vec3 itrf_position;
  Vec3 itrf_velocity;

  GCRFToITRF(gcrf.observation_time,
             gcrf.position.cartesian(),
             gcrf.velocity.cartesian_or({0, 0, 0}),
             itrf_position,
             itrf_velocity);

  ITRF itrf;

  itrf.observation_time = gcrf.observation_time;
  itrf.position = itrf_position;

  if (gcrf.velocity.has_value()) {
    itrf.velocity = itrf_velocity;
  }

  return itrf;
}

auto ITRF::FromTEME(const TEME& teme) -> ITRF {
  Vec3 itrf_position;
  Vec3 itrf_velocity;

  TEMEToITRF(teme.observation_time,
             teme.position.cartesian(),
             teme.velocity.cartesian_or({0, 0, 0}),
             itrf_position,
             itrf_velocity);

  ITRF itrf;

  itrf.observation_time = teme.observation_time;
  itrf.position = itrf_position;

  if (teme.velocity.has_value()) {
    itrf.velocity = itrf_velocity;
  }

  return itrf;
}

namespace {

consteval auto GetEllipsoid() -> Earth::Ellipsoid {
  // Use default ellipsoid from Astropy.
  return Earth::GetEllipsoid<Earth::System::WGS84>::Get();
}

}  // namespace

auto ITRF::FromGeodetic(const Geodetic& geodetic) -> ITRF {
  // From geodetic to ECEF coordinates
  //
  // https://en.wikipedia.org/wiki/Geographic_coordinate_conversion#From_geodetic_to_ECEF_coordinates

  const Earth::Ellipsoid ellipsoid = GetEllipsoid();

  const double a = ellipsoid.a;  // equatorial radius a
  const double f = ellipsoid.f;  // flattening of the ellipsoid f

  const double phi = geodetic.latitude;      // latitude ϕ
  const double lambda = geodetic.longitude;  // longitude λ
  const double h = geodetic.height;          // height h

  double sin_phi, cos_phi;
  SinCos(phi, sin_phi, cos_phi);

  double sin_lambda, cos_lambda;
  SinCos(lambda, sin_lambda, cos_lambda);

  // Square of the first numerical eccentricity of the ellipsoid.
  //
  //   https://en.wikipedia.org/wiki/Flattening
  const double e2 = (2.0 - f) * f;

  // Calculate N(ϕ).
  const double N = a / Sqrt(1 - e2 * sin_phi * sin_phi);

  const double r = (N + h) * cos_phi;

  const double X = r * cos_lambda;
  const double Y = r * sin_lambda;

  // NOTE: Canonically it is `Z = ((1.0 - e2) * N + h) * sin_phi` but using it
  // as-is leads to precision issues with th regression tests on Raspberry Pi 4
  // and GCC-10 when building in release mode. Reshuffling the terms leads to an
  // extra multiplication, but solves the failing test.
  const double Z = (1.0 - e2) * N * sin_phi + h * sin_phi;

  return ITRF(
      {.observation_time = geodetic.observation_time, .position{X, Y, Z}});
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
