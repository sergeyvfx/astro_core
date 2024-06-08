// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/teme.h"

#include "astro_core/coordinate/frame_transform.h"
#include "astro_core/coordinate/itrf.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto TEME::FromITRF(const ITRF& itrf) -> TEME {
  Vec3 teme_position;
  Vec3 teme_velocity;

  ITRFToTEME(itrf.observation_time,
             itrf.position.GetCartesian(),
             itrf.velocity.GetCartesianOr({0, 0, 0}),
             teme_position,
             teme_velocity);

  TEME teme;

  teme.position = teme_position;
  teme.observation_time = itrf.observation_time;

  if (itrf.velocity.HasValue()) {
    teme.velocity = teme_velocity;
  }

  return teme;
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
