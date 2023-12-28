// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/geographic.h"

#include "astro_core/coordinate/geodetic.h"
#include "astro_core/coordinate/qth.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto Geographic::FromGeodetic(const Geodetic& geodetic) -> Geographic {
  return Geographic({
      .latitude = geodetic.latitude,
      .longitude = geodetic.longitude,
  });
}

auto Geographic::FromQTH(const QTH& qth) -> Geographic {
  return Geographic({
      .latitude = qth.CalculateLatitude(),
      .longitude = qth.CalculateLongitude(),
  });
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
