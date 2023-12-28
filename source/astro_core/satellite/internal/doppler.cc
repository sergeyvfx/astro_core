// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/satellite/doppler.h"

#include "astro_core/base/constants.h"
#include "astro_core/coordinate/itrf.h"
#include "astro_core/numeric/numeric.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto CalculateDopplerFactor(const ITRF& site, const ITRF& satellite) -> double {
  // Normalized direction from the observer to the satellite.
  const Vec3 direction =
      Vec3(satellite.position.cartesian()) - Vec3(site.position.cartesian());
  const Vec3 normalized_direction = direction.Normalized();

  // Project velocities of site and the satellite.
  // The site is typically stationary, but this is not enforced by the API,
  // so take its velocity into account as well.
  const double v_site =
      Vec3(site.velocity.cartesian_or({0, 0, 0})).Dot(normalized_direction);
  const double v_satellite = Vec3(satellite.velocity.cartesian_or({0, 0, 0}))
                                 .Dot(normalized_direction);

  // Velocity of the satellite relative to the site.
  const double v_relative = v_satellite - v_site;

  return constants::kSpeedOfLight / (constants::kSpeedOfLight + v_relative);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
