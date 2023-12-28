// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/footprint.h"

#include "astro_core/base/constants.h"
#include "astro_core/coordinate/geodetic.h"
#include "astro_core/earth/earth.h"
#include "astro_core/math/math.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

// Get the radius of a great circle on the surface of Earth from within which
// the satellite elevation is above an observer horizon.
// Measured in radians.
auto CalculateFootprintRadius(const Geodetic& satellite_position) -> double {
  const Earth::Ellipsoid ellipsoid =
      Earth::GetEllipsoid<Earth::System::WGS84>::Get();
  // For the simplicity use the major axis only. This radius is only used for
  // a footprint estimation, which is already an approximation and will have
  // other challenges if we're to take more correct shape into account: like
  // mountains.
  const double earth_radius = ellipsoid.a;

  return ArcCos(earth_radius / (earth_radius + satellite_position.height));
}

// Wrap the longitude to the -pi .. +pi range.
auto WrapLongitude(const double longitude) -> double {
  if (longitude < -constants::pi) {
    return longitude + 2 * constants::pi;
  }

  if (longitude > constants::pi) {
    return longitude - 2 * constants::pi;
  }

  return longitude;
}

}  // namespace

void CalculateSatelliteFootprint(const Geodetic& satellite_position,
                                 std::span<Geographic> footprint) {
  // The implementation follow formulas and algorithm described in
  //
  //   Lat/Lon Coordinates of a Circle on a Sphere
  //   https://math.stackexchange.com/a/2550930
  //
  // The naming follows the formula in the post.

  const double d = CalculateFootprintRadius(satellite_position);

  const double theta0 = satellite_position.latitude;
  const double phi0 = satellite_position.longitude;

  double theta_min = theta0 - d;
  double theta_max = theta0 + d;

  // Wrap the coordinate around the pole.
  if (theta_max > constants::pi / 2) {
    theta_max = constants::pi - theta_max;
  }
  if (theta_min < -constants::pi / 2) {
    theta_min = -constants::pi - theta_min;
  }

  // The number of points to calculate.
  // Note that the loop goes to the halp of the footprint points, because due to
  // the nature of equation every loop iteration produces two points of a
  // footprint.
  const size_t num_points = footprint.size();
  const size_t half_num_points = num_points / 2;

  // Step of theta in radians per the loop iteration.
  const double theta_step =
      (theta_max - theta_min) / double(half_num_points - 1);

  // Pre-calculate terms which do not change over the loop.
  const double sin_d_2 = Sin(d / 2);
  const double cos_theta0 = Cos(theta0);

  for (size_t i = 0; i < half_num_points; ++i) {
    const double theta = theta_min + theta_step * double(i);

    const double b = Sin((theta - theta0) / 2);
    const double c = Cos(theta) * cos_theta0;

    // Due to precision issues with calculations above it is possible that the
    // value is either slightly negative, or slightly above 1 (within very small
    // epsilon like 1e-17). Clamp the value, so that it does not lead to non-
    // finite results.
    const double det = Saturate((sin_d_2 * sin_d_2 - b * b) / c);

    const double term = 2 * ArcSin(Sqrt(det));

    const double phi_1 = WrapLongitude(phi0 + term);
    const double phi_2 = WrapLongitude(phi0 - term);

    footprint[i] = Geographic({.latitude = theta, .longitude = phi_1});
    footprint[2 * half_num_points - i - 1] =
        Geographic({.latitude = theta, .longitude = phi_2});
  }

  // Append extra point, so that all stored are valid.
  if (num_points % 2) {
    footprint.back() = footprint.front();
  }
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
