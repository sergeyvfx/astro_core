// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/representation.h"

#include <optional>

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/spherical.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Eq;
using testing::Pointwise;

TEST(Representation, BasicCartesian) {
  // Default constructor.
  {
    const Representation<Cartesian> representation;
    EXPECT_TRUE(representation.HasValue());
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(0, 0, 0)));
  }

  // Simple constructor in the same representation.
  {
    const Representation<Cartesian> representation(Cartesian(1, 2, 3));
    EXPECT_TRUE(representation.HasValue());
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(1, 2, 3)));
  }

  // Optional value.
  {
    Representation<std::optional<Cartesian>> representation;
    EXPECT_FALSE(representation.HasValue());
    representation = Cartesian(1, 2, 3);
    EXPECT_TRUE(representation.HasValue());
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(1, 2, 3)));
  }
}

TEST(Representation, BasicSpherical) {
  // Default constructor.
  {
    const Representation<Spherical> representation;
    EXPECT_TRUE(representation.HasValue());
    EXPECT_EQ(representation.GetSpherical().latitude, 0);
    EXPECT_EQ(representation.GetSpherical().longitude, 0);
    EXPECT_EQ(representation.GetSpherical().distance, 0);
  }

  // Simple constructor in the same representation.
  {
    const Representation<Spherical> representation(
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3}));
    EXPECT_TRUE(representation.HasValue());
    EXPECT_EQ(representation.GetSpherical().latitude, 0.1);
    EXPECT_EQ(representation.GetSpherical().longitude, 0.2);
    EXPECT_EQ(representation.GetSpherical().distance, 0.3);
  }

  // Optional value.
  {
    Representation<std::optional<Spherical>> representation;
    EXPECT_FALSE(representation.HasValue());
    representation =
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});
    EXPECT_TRUE(representation.HasValue());
    EXPECT_EQ(representation.GetSpherical().latitude, 0.1);
    EXPECT_EQ(representation.GetSpherical().longitude, 0.2);
    EXPECT_EQ(representation.GetSpherical().distance, 0.3);
  }
}

TEST(Representation, GetCartesian) {
  {
    const Representation<Cartesian> representation(Cartesian{1, 2, 3});
    const Cartesian cartesian = representation.GetCartesian();
    EXPECT_THAT(Vec3(cartesian), Pointwise(Eq(), Vec3(1, 2, 3)));
  }

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.spherical_to_cartesian(r=0.3, lat=0.1, lon=0.2)
  // (<Quantity 0.2925511>, <Quantity 0.05930304>, <Quantity 0.02995002>)
  {
    const Representation<Spherical> representation(
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3}));
    const Cartesian cartesian = representation.GetCartesian();
    EXPECT_THAT(
        Vec3(cartesian),
        Pointwise(DoubleNear(1e-6), Vec3(0.2925511, 0.05930304, 0.02995002)));
  }
}

TEST(Representation, GetSpherical) {
  // >>> import astropy.coordinates
  // >>> astropy.coordinates.cartesian_to_spherical(
  // ...     0.2925511, 0.05930304, 0.02995002)
  // (<Quantity 0.3>, <Latitude 0.09999998 rad>, <Longitude 0.19999999 rad>)
  {
    const Representation<Cartesian> representation(
        Cartesian(0.2925511, 0.05930304, 0.02995002));
    const Spherical spherical = representation.GetSpherical();
    EXPECT_NEAR(spherical.latitude, 0.1, 1e-6);
    EXPECT_NEAR(spherical.longitude, 0.2, 1e-6);
    EXPECT_NEAR(spherical.distance, 0.3, 1e-6);
  }

  {
    const Representation<Spherical> representation(
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3}));
    const Spherical spherical = representation.GetSpherical();
    EXPECT_NEAR(spherical.latitude, 0.1, 1e-6);
    EXPECT_NEAR(spherical.longitude, 0.2, 1e-6);
    EXPECT_NEAR(spherical.distance, 0.3, 1e-6);
  }
}

TEST(Representation, GetCartesianOr) {
  // Position.
  {
    Representation<std::optional<Cartesian>> representation;

    EXPECT_EQ(representation.GetCartesianOr({3, 2, 1}), Cartesian(3, 2, 1));

    representation = Cartesian(1, 2, 3);
    EXPECT_EQ(representation.GetCartesianOr({3, 2, 1}), Cartesian(1, 2, 3));
  }

  // Differential.
  {
    Representation<std::optional<CartesianDifferential>> representation;

    EXPECT_EQ(representation.GetCartesianOr({3, 2, 1}),
              CartesianDifferential(3, 2, 1));

    representation = CartesianDifferential(1, 2, 3);
    EXPECT_EQ(representation.GetCartesianOr({3, 2, 1}),
              CartesianDifferential(1, 2, 3));
  }
}

TEST(Representation, GetSphericalOr) {
  // Position.
  {
    Representation<std::optional<Spherical>> representation;

    EXPECT_EQ(representation.GetSphericalOr(
                  Spherical({.latitude = 3, .longitude = 2, .distance = 1})),
              Spherical({.latitude = 3, .longitude = 2, .distance = 1}));

    representation = Spherical({.latitude = 1, .longitude = 2, .distance = 3});
    EXPECT_EQ(representation.GetSphericalOr(
                  Spherical({.latitude = 3, .longitude = 2, .distance = 1})),
              Spherical({.latitude = 1, .longitude = 2, .distance = 3}));
  }

  // Velocity.
  {
    Representation<std::optional<SphericalDifferential>> representation;

    EXPECT_EQ(representation.GetSphericalOr(SphericalDifferential(
                  {.d_latitude = 3, .d_longitude = 2, .d_distance = 1})),
              SphericalDifferential(
                  {.d_latitude = 3, .d_longitude = 2, .d_distance = 1}));

    representation = SphericalDifferential(
        {.d_latitude = 1, .d_longitude = 2, .d_distance = 3});
    EXPECT_EQ(representation.GetSphericalOr(SphericalDifferential(
                  {.d_latitude = 3, .d_longitude = 2, .d_distance = 1})),
              SphericalDifferential(
                  {.d_latitude = 1, .d_longitude = 2, .d_distance = 3}));
  }
}

TEST(Representation, AssignCartesianToCartesian) {
  // Cartesian = Cartesian
  {
    Representation<Cartesian> representation;
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(0, 0, 0)));

    representation = Cartesian(1, 2, 3);
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(1, 2, 3)));
  }

  // std::optional<Cartesian> = Cartesian
  {
    Representation<std::optional<Cartesian>> representation;
    EXPECT_FALSE(representation.HasValue());

    representation = Cartesian(1, 2, 3);
    EXPECT_TRUE(representation.HasValue());
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(1, 2, 3)));
  }

  // std::optional<Cartesian> = std::optional<Cartesian>
  {
    Representation<std::optional<Cartesian>> representation;
    EXPECT_FALSE(representation.HasValue());

    representation = Cartesian(1, 2, 3);
    EXPECT_TRUE(representation.HasValue());
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(1, 2, 3)));

    representation = std::nullopt;
    EXPECT_FALSE(representation.HasValue());

    representation = std::optional(Cartesian(1, 2, 3));
    EXPECT_TRUE(representation.HasValue());
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(1, 2, 3)));
  }
}

TEST(Representation, AssignSphericalToCartesian) {
  // Cartesian = Spherical
  {
    Representation<Cartesian> representation;
    EXPECT_THAT(Vec3(representation.GetCartesian()),
                Pointwise(Eq(), Vec3(0, 0, 0)));

    representation =
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});
    EXPECT_EQ(representation.GetSpherical().latitude, 0.1);
    EXPECT_EQ(representation.GetSpherical().longitude, 0.2);
    EXPECT_EQ(representation.GetSpherical().distance, 0.3);
  }

  // std::optional<Cartesian> = Cartesian
  {
    Representation<std::optional<Cartesian>> representation;
    EXPECT_FALSE(representation.HasValue());

    representation =
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});
    EXPECT_TRUE(representation.HasValue());
    EXPECT_EQ(representation.GetSpherical().latitude, 0.1);
    EXPECT_EQ(representation.GetSpherical().longitude, 0.2);
    EXPECT_EQ(representation.GetSpherical().distance, 0.3);
  }

  // std::optional<Cartesian> = std::optional<Spherical>
  {
    Representation<std::optional<Cartesian>> representation;
    EXPECT_FALSE(representation.HasValue());

    representation =
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});
    EXPECT_TRUE(representation.HasValue());
    EXPECT_EQ(representation.GetSpherical().latitude, 0.1);
    EXPECT_EQ(representation.GetSpherical().longitude, 0.2);
    EXPECT_EQ(representation.GetSpherical().distance, 0.3);

    representation = std::nullopt;
    EXPECT_FALSE(representation.HasValue());

    representation = std::optional(
        Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3}));
    EXPECT_EQ(representation.GetSpherical().latitude, 0.1);
    EXPECT_EQ(representation.GetSpherical().longitude, 0.2);
    EXPECT_EQ(representation.GetSpherical().distance, 0.3);
  }
}

}  // namespace astro_core
