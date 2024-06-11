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

TEST(CartesianDifferential, ToSpherical) {
  {
    // Below goes verification code using Numpy and Astropy.
    // For the verification code which uses ERFA library see
    // source/validate/cartesian_to_spherical.c.
    //
    // clang-format off
    //   >>> import numpy as np
    //   >>> np.set_printoptions(precision=18, suppress=True)
    //   >>> from astropy.coordinates import CartesianDifferential
    //   >>> from astropy.coordinates import CartesianRepresentation
    //   >>> from astropy.coordinates import SphericalDifferential
    //   >>> from astropy.coordinates import SphericalRepresentation
    //   >>> from astropy import units as u
    //   >>> cartesian_pos = CartesianRepresentation(1, 2, 3) * u.m
    //   >>> cartesian_vel = CartesianDifferential(0.4, 0.5, 0.6) * u.m / u.s
    //   >>> cartesian_vel.represent_as(SphericalDifferential, base=cartesian_pos)
    //   <SphericalDifferential (d_lon, d_lat, d_distance) in (rad / s, rad / s, m / s)
    //       (-0.059999999999999984, -0.038332593899996416, 0.855235974119758)>
    // clang-format on
    //
    // Verification using Python's ERFA library:
    //   >>> import numpy as np
    //   >>> np.set_printoptions(precision=18, suppress=True)
    //   >>> import numpy as np
    //   >>> import erfa
    //   >>> pv = np.array(([1.0, 2.0, 3.0], [0.4, 0.5, 0.6]),
    //   ...               dtype=erfa.dt_pv)
    //   >>> erfa.pv2s(pv)
    //   (1.1071487177940904, 0.9302740141154721, 3.7416573867739413,
    //    -0.06000000000000001, -0.03833259389999637, 0.8552359741197579)

    const Cartesian position_cartesian(1, 2, 3);
    const CartesianDifferential velocity_cartesian(0.4, 0.5, 0.6);
    const SphericalDifferential velocity_spherical =
        velocity_cartesian.ToSpherical(position_cartesian);

    EXPECT_NEAR(velocity_spherical.d_latitude, -0.03833259389999637, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_longitude, -0.06000000000000001, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_distance, 0.8552359741197579, 1e-12);
  }

  // Zero position, non-zero derivatives.
  {
    //   >>> import numpy as np
    //   >>> np.set_printoptions(precision=18, suppress=True)
    //   >>> import numpy as np
    //   >>> import erfa
    //   >>> pv = np.array(([0.0, 0.0, 0.0], [0.4, 0.5, 0.6]),
    //   ...               dtype=erfa.dt_pv)
    //   >>> erfa.pv2s(pv)
    //   (0.8960553845713439, 0.7529077706294455, 0.0,
    //    0.0, 2.6571103042006167e-17, 0.8774964387392122)

    const Cartesian position_cartesian(0, 0, 0);
    const CartesianDifferential velocity_cartesian(0.4, 0.5, 0.6);
    const SphericalDifferential velocity_spherical =
        velocity_cartesian.ToSpherical(position_cartesian);

    EXPECT_NEAR(velocity_spherical.d_latitude, 2.6571103042006167e-17, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_longitude, 0.0, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_distance, 0.8774964387392122, 1e-12);
  }

  // Zero position, zero angular derivatives.
  {
    //   >>> import numpy as np
    //   >>> np.set_printoptions(precision=18, suppress=True)
    //   >>> import numpy as np
    //   >>> import erfa
    //   >>> pv = np.array(([0.0, 0.0, 0.0], [0.0, 0.0, 0.6]),
    //   ...               dtype=erfa.dt_pv)
    //   >>> erfa.pv2s(pv)
    //   (0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.6)

    const Cartesian position_cartesian(0, 0, 0);
    const CartesianDifferential velocity_cartesian(0.0, 0.0, 0.6);
    const SphericalDifferential velocity_spherical =
        velocity_cartesian.ToSpherical(position_cartesian);

    EXPECT_NEAR(velocity_spherical.d_latitude, 0.0, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_longitude, 0.0, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_distance, 0.6, 1e-12);
  }

  // All zero.
  {
    //   >>> import numpy as np
    //   >>> np.set_printoptions(precision=18, suppress=True)
    //   >>> import erfa
    //   >>> pv = np.array(([0.0, 0.0, 0.0], [0.0, 0.0, 0.0]),
    //   ...               dtype=erfa.dt_pv)
    //   >>> erfa.pv2s(pv)
    //   (0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

    const Cartesian position_cartesian(0, 0, 0);
    const CartesianDifferential velocity_cartesian(0.0, 0.0, 0.0);
    const SphericalDifferential velocity_spherical =
        velocity_cartesian.ToSpherical(position_cartesian);

    EXPECT_NEAR(velocity_spherical.d_latitude, 0.0, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_longitude, 0.0, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_distance, 0.0, 1e-12);
  }

  // Test conversion when position is in spherical coordinates.
  {
    const Spherical position_spherical = Cartesian(1, 2, 3).ToSpherical();
    const CartesianDifferential velocity_cartesian(0.4, 0.5, 0.6);
    const SphericalDifferential velocity_spherical =
        velocity_cartesian.ToSpherical(position_spherical);

    EXPECT_NEAR(velocity_spherical.d_latitude, -0.03833259389999637, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_longitude, -0.06000000000000001, 1e-12);
    EXPECT_NEAR(velocity_spherical.d_distance, 0.8552359741197579, 1e-12);
  }
}

}  // namespace astro_core
