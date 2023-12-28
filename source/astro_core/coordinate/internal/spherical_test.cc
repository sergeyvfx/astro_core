// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/spherical.h"

#include "astro_core/base/constants.h"
#include "astro_core/coordinate/cartesian.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(Spherical, Construct) {
  {
    const Spherical point;
    EXPECT_EQ(point.latitude, 0.0);
    EXPECT_EQ(point.longitude, 0.0);
    EXPECT_EQ(point.distance, 0.0);
  }

  {
    const Spherical point({
        .latitude = 1,
        .longitude = 2,
        .distance = 3,
    });

    EXPECT_NEAR(point.latitude, 1.0, 1e-12);
    EXPECT_NEAR(point.longitude, 2.0, 1e-12);
    EXPECT_NEAR(point.distance, 3.0, 1e-12);
  }
}

TEST(Spherical, ToCartesian) {
  const double pi = constants::pi;

  EXPECT_THAT(Vec3(Spherical().ToCartesian()),
              Pointwise(DoubleNear(1e-12), Vec3(0, 0, 0)));

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.spherical_to_cartesian(r=1, lat=0, lon=0)
  // (<Quantity 1.>, <Quantity 0.>, <Quantity 0.>)
  EXPECT_THAT(Vec3(Spherical({.latitude = 0, .longitude = 0, .distance = 1})
                       .ToCartesian()),
              Pointwise(DoubleNear(1e-12), Vec3(1, 0, 0)));

  // >>> import astropy.coordinates
  // >>> import math
  // >>> astropy.coordinates.spherical_to_cartesian(r=1, lat=0, lon=math.pi/2)
  // (<Quantity 1.>, <Quantity 0.>, <Quantity 0.>)
  EXPECT_THAT(
      Vec3(Spherical({.latitude = 0, .longitude = pi / 2, .distance = 1})
               .ToCartesian()),
      Pointwise(DoubleNear(1e-12), Vec3(0, 1, 0)));

  // >>> import astropy.coordinates
  // >>> import math
  // >>> astropy.coordinates.spherical_to_cartesian(r=1, lat=math.pi/2, lon=0)
  // (<Quantity 6.123234e-17>, <Quantity 0.>, <Quantity 1.>)
  EXPECT_THAT(
      Vec3(Spherical({.latitude = pi / 2, .longitude = 0, .distance = 1})
               .ToCartesian()),
      Pointwise(DoubleNear(1e-12), Vec3(0, 0, 1)));

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.spherical_to_cartesian(r=0.3, lat=0.1, lon=0.2)
  // (<Quantity 0.2925511>, <Quantity 0.05930304>, <Quantity 0.02995002>)
  EXPECT_THAT(
      Vec3(Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3})
               .ToCartesian()),
      Pointwise(DoubleNear(1e-6), Vec3(0.2925511, 0.05930304, 0.02995002)));
}

TEST(SphericalDifferential, Construct) {
  {
    const SphericalDifferential velocity;
    EXPECT_EQ(velocity.d_latitude, 0.0);
    EXPECT_EQ(velocity.d_longitude, 0.0);
    EXPECT_EQ(velocity.d_distance, 0.0);
  }

  {
    const SphericalDifferential velocity({
        .d_latitude = 1,
        .d_longitude = 2,
        .d_distance = 3,
    });

    EXPECT_NEAR(velocity.d_latitude, 1.0, 1e-12);
    EXPECT_NEAR(velocity.d_longitude, 2.0, 1e-12);
    EXPECT_NEAR(velocity.d_distance, 3.0, 1e-12);
  }
}

}  // namespace astro_core
