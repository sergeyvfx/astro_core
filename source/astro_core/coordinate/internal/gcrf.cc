// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/gcrf.h"

#include "astro_core/coordinate/frame_transform.h"
#include "astro_core/coordinate/itrf.h"
#include "astro_core/numeric/numeric.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto GCRF::FromITRF(const ITRF& itrf) -> GCRF {
  Vec3 gcrf_position;
  Vec3 gcrf_velocity;

  ITRFToGCRF(itrf.observation_time,
             itrf.position.GetCartesian(),
             itrf.velocity.GetCartesianOr({0, 0, 0}),
             gcrf_position,
             gcrf_velocity);

  GCRF gcrf;

  gcrf.observation_time = itrf.observation_time;
  gcrf.position = gcrf_position;

  if (itrf.velocity.HasValue()) {
    gcrf.velocity = gcrf_velocity;
  }

  return gcrf;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
