// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/geocentric_radius.h"

#include "astro_core/earth/earth.h"
#include "astro_core/math/math.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(earth, GeocentricRadius) {
  const Earth::Ellipsoid ellipsoid =
      Earth::GetEllipsoid<Earth::System::WGS84>::Get();

  // TODO(sergey): Cross-check the value.
  EXPECT_NEAR(GeocentricRadius(ellipsoid, DegreesToRadians(50.0)),
              6365631.517537280917167664,
              1e-12);
}

}  // namespace astro_core
