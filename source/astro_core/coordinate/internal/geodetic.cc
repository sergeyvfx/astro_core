// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/geodetic.h"

#include "astro_core/base/constants.h"
#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/geographic.h"
#include "astro_core/coordinate/itrf.h"
#include "astro_core/earth/earth.h"
#include "astro_core/math/math.h"

// References:
//
// [Fukushima2006] Toshio Fukushima. 2006. Transformation from Cartesian to
//     Geodetic Coordinates Accelerated by Halley’s Method. Journal of Geodesy
//     79, (February 2006), 689–693.
//     DOI:https://doi.org/10.1007/s00190-006-0023-2

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

consteval auto GetEllipsoid() -> Earth::Ellipsoid {
  // Use default ellipsoid from Astropy.
  return Earth::GetEllipsoid<Earth::System::WGS84>::Get();
}

}  // namespace

auto Geodetic::FromGeocentric(const Vec3& geocentric,
                              const Time& observation_time) -> Geodetic {
  const Earth::Ellipsoid ellipsoid = GetEllipsoid();

  Geodetic geodetic;

  geodetic.observation_time = observation_time;

  const double x = geocentric(0);
  const double y = geocentric(1);
  const double z = geocentric(2);

  const double a = ellipsoid.a;
  const double f = ellipsoid.f;

  // Early calculation of the longitude.
  // It is always possible to calculate, even for coordinates near the poles.
  geodetic.longitude = ArcTan2(y, x);

  // The conversion algorithm follows [Fukushima2006].

  const double abs_z = Abs(z);
  const double sign_z = z < 0.0 ? -1.0 : 1.0;

  const double p = Sqrt(x * x + y * y);

  // Convert flattening to squared eccentricity:
  //
  //  https://en.wikipedia.org/wiki/Flattening
  //
  // The equation is reversed, expanded, and simplified.
  // The Eq. (2) defines E ≡ e^2 .
  const double E = (2.0 - f) * f;

  // ec ≡ Sqrt(1 − e^2).
  const double ec = Sqrt(1.0 - E);

  const double b = a * ec;  // // From Eq. (8).

  if (p < 1e-18) {
    geodetic.latitude = sign_z * constants::pi / 2;
    geodetic.height = abs_z - b;
    return geodetic;
  }

  // Eq. (2).
  const double P = p / a;
  const double Z = ec * abs_z / a;

  // Eq. (17).
  const double S0 = Z;
  const double C0 = ec * P;

  const double A0 = Sqrt(S0 * S0 + C0 * C0);                  // Eq. (14).
  const double D0 = Z * (A0 * A0 * A0) + E * (S0 * S0 * S0);  // Eq. (12).
  const double F0 = P * (A0 * A0 * A0) - E * (C0 * C0 * C0);  // Eq. (13).
  const double B0 = 1.5 * E * S0 * C0 * C0 *
                    ((P * S0 - Z * C0) * A0 - E * S0 * C0);  // Eq. (15).

  const double S1 = D0 * F0 - B0 * S0;  // Eq. (10).
  const double C1 = F0 * F0 - B0 * C0;  // Eq. (11).

  const double A1 = Sqrt(S1 * S1 + C1 * C1);  // Eq. (14).

  const double Cc = ec * C1;  // Eq. (21).

  geodetic.latitude = sign_z * ArcTan(S1 / Cc);  // Eq. (19).
  geodetic.height =
      (p * Cc + abs_z * S1 - b * A1) / Sqrt(Cc * Cc + S1 * S1);  // Eq. (20).

  return geodetic;
}

auto Geodetic::FromGeographic(const Geographic& geographic,
                              const Time& observation_time) -> Geodetic {
  return Geodetic({
      .observation_time = observation_time,
      .latitude = geographic.latitude,
      .longitude = geographic.longitude,
      .height = 0.0,
  });
}

auto Geodetic::FromITRF(const ITRF& itrf) -> Geodetic {
  return FromGeocentric(itrf.position.GetCartesian(), itrf.observation_time);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
