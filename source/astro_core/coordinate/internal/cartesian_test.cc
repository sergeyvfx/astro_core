// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/cartesian.h"

#include "astro_core/coordinate/spherical.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Eq;
using testing::Pointwise;

TEST(Cartesian, Construct) {
  {
    const Cartesian point;
    EXPECT_THAT(Vec3(point), Pointwise(Eq(), Vec3(0, 0, 0)));
  }

  {
    const Cartesian point(1, 2, 3);

    EXPECT_THAT(Vec3(point), Pointwise(DoubleNear(1e-12), Vec3(1, 2, 3)));

    EXPECT_NEAR(point.x, 1.0, 1e-12);
    EXPECT_NEAR(point.y, 2.0, 1e-12);
    EXPECT_NEAR(point.z, 3.0, 1e-12);
  }
}

TEST(Cartesian, ToSpherical) {
  const double pi = constants::pi;

  {
    const Spherical spherical = Cartesian().ToSpherical();
    EXPECT_NEAR(spherical.latitude, 0, 1e-12);
    EXPECT_NEAR(spherical.longitude, 0, 1e-12);
    EXPECT_NEAR(spherical.distance, 0, 1e-12);
  }

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.cartesian_to_spherical(1, 0, 0)
  // (<Quantity 1.>, <Latitude 0. rad>, <Longitude 0. rad>)
  {
    const Spherical spherical = Cartesian(1, 0, 0).ToSpherical();
    EXPECT_NEAR(spherical.latitude, 0, 1e-12);
    EXPECT_NEAR(spherical.longitude, 0, 1e-12);
    EXPECT_NEAR(spherical.distance, 1, 1e-12);
  }

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.cartesian_to_spherical(0, 1, 0)
  // (<Quantity 1.>, <Latitude 0. rad>, <Longitude 1.57079633 rad>)
  {
    const Spherical spherical = Cartesian(0, 1, 0).ToSpherical();
    EXPECT_NEAR(spherical.latitude, 0, 1e-12);
    EXPECT_NEAR(spherical.longitude, pi / 2, 1e-12);
    EXPECT_NEAR(spherical.distance, 1, 1e-12);
  }

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.cartesian_to_spherical(0, 0, 1)
  // (<Quantity 1.>, <Latitude 1.57079633 rad>, <Longitude 0. rad>)
  {
    const Spherical spherical = Cartesian(0, 0, 1).ToSpherical();
    EXPECT_NEAR(spherical.latitude, pi / 2, 1e-12);
    EXPECT_NEAR(spherical.longitude, 0, 1e-12);
    EXPECT_NEAR(spherical.distance, 1, 1e-12);
  }

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.cartesian_to_spherical(
  // ...     0.2925511, 0.05930304, 0.02995002)
  // (<Quantity 0.3>, <Latitude 0.09999998 rad>, <Longitude 0.19999999 rad>)
  {
    const Spherical spherical =
        Cartesian(0.2925511, 0.05930304, 0.02995002).ToSpherical();
    EXPECT_NEAR(spherical.latitude, 0.1, 1e-6);
    EXPECT_NEAR(spherical.longitude, 0.2, 1e-6);
    EXPECT_NEAR(spherical.distance, 0.3, 1e-6);
  }
}

TEST(CartesianDifferential, Construct) {
  {
    const CartesianDifferential velocity;
    EXPECT_THAT(Vec3(velocity), Pointwise(Eq(), Vec3(0, 0, 0)));
  }

  {
    const CartesianDifferential velocity(1, 2, 3);

    EXPECT_THAT(Vec3(velocity), Pointwise(DoubleNear(1e-12), Vec3(1, 2, 3)));

    EXPECT_NEAR(velocity.d_x, 1.0, 1e-12);
    EXPECT_NEAR(velocity.d_y, 2.0, 1e-12);
    EXPECT_NEAR(velocity.d_z, 3.0, 1e-12);
  }
}

}  // namespace astro_core
