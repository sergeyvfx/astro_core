// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/representation_view.h"

#include <optional>

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/spherical.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

////////////////////////////////////////////////////////////////////////////////
// Const representation.

TEST(ConstRepresentationView, Basic) {
  Cartesian point;
  ConstRepresentationView view(point);

  EXPECT_THAT(Vec3(point), Pointwise(DoubleNear(1e-12), Vec3(0, 0, 0)));

  point = Cartesian(1, 2, 3);

  EXPECT_THAT(Vec3(point), Pointwise(DoubleNear(1e-12), Vec3(1, 2, 3)));

  EXPECT_NEAR(view.cartesian().x, 1.0, 1e-12);
  EXPECT_NEAR(view.cartesian().y, 2.0, 1e-12);
  EXPECT_NEAR(view.cartesian().z, 3.0, 1e-12);
}

TEST(ConstRepresentationView, ToCartesian) {
  {
    const Cartesian point(1, 2, 3);
    const ConstRepresentationView view(point);
    const Cartesian cartesian = view.cartesian();
    EXPECT_THAT(Vec3(cartesian), Pointwise(DoubleNear(1e-12), Vec3(1, 2, 3)));
  }

  // >>> import astropy.coordinates
  // >>> astropy.coordinates.spherical_to_cartesian(r=0.3, lat=0.1, lon=0.2)
  // (<Quantity 0.2925511>, <Quantity 0.05930304>, <Quantity 0.02995002>)
  {
    const Spherical point({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});
    const ConstRepresentationView view(point);
    const Cartesian cartesian = view.cartesian();
    EXPECT_THAT(
        Vec3(cartesian),
        Pointwise(DoubleNear(1e-6), Vec3(0.2925511, 0.05930304, 0.02995002)));
  }
}

TEST(ConstRepresentationView, ToSpherical) {
  // >>> import astropy.coordinates
  // >>> astropy.coordinates.cartesian_to_spherical(
  // ...     0.2925511, 0.05930304, 0.02995002)
  // (<Quantity 0.3>, <Latitude 0.09999998 rad>, <Longitude 0.19999999 rad>)
  {
    const Cartesian point(0.2925511, 0.05930304, 0.02995002);
    const ConstRepresentationView view(point);
    const Spherical spherical = view.spherical();
    EXPECT_NEAR(spherical.latitude, 0.1, 1e-6);
    EXPECT_NEAR(spherical.longitude, 0.2, 1e-6);
    EXPECT_NEAR(spherical.distance, 0.3, 1e-6);
  }

  {
    const Spherical point({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});
    const ConstRepresentationView view(point);
    const Spherical spherical = view.spherical();
    EXPECT_NEAR(spherical.latitude, 0.1, 1e-6);
    EXPECT_NEAR(spherical.longitude, 0.2, 1e-6);
    EXPECT_NEAR(spherical.distance, 0.3, 1e-6);
  }
}

TEST(ConstRepresentationView, Optional) {
  std::optional<Cartesian> point;
  ConstRepresentationView view(point);

  point = Cartesian(1, 2, 3);

  EXPECT_NEAR(view.cartesian().x, 1.0, 1e-12);
  EXPECT_NEAR(view.cartesian().y, 2.0, 1e-12);
  EXPECT_NEAR(view.cartesian().z, 3.0, 1e-12);
}

TEST(ConstRepresentationView, HasValue) {
  {
    const Cartesian point;
    ConstRepresentationView view(point);

    EXPECT_TRUE(view.has_value());
  }

  {
    std::optional<Cartesian> point;
    ConstRepresentationView view(point);

    EXPECT_FALSE(view.has_value());

    point = Cartesian(1, 2, 3);

    EXPECT_TRUE(view.has_value());
  }
}

TEST(ConstRepresentationView, cartesian_or) {
  std::optional<Cartesian> point;

  ConstRepresentationView view(point);

  EXPECT_EQ(view.cartesian_or({3, 2, 1}), Cartesian(3, 2, 1));

  point = Cartesian(1, 2, 3);

  EXPECT_EQ(view.cartesian_or({3, 2, 1}), Cartesian(1, 2, 3));
}

TEST(ConstRepresentationView, spherical_or) {
  std::optional<Spherical> point;

  ConstRepresentationView view(point);

  EXPECT_EQ(view.spherical_or(
                Spherical({.latitude = 3, .longitude = 2, .distance = 1})),
            Spherical({.latitude = 3, .longitude = 2, .distance = 1}));

  point = Spherical({.latitude = 1, .longitude = 2, .distance = 3});

  EXPECT_EQ(view.spherical_or(
                Spherical({.latitude = 3, .longitude = 2, .distance = 1})),
            Spherical({.latitude = 1, .longitude = 2, .distance = 3}));
}

////////////////////////////////////////////////////////////////////////////////
// Mutable representation.

TEST(MutableRepresentationView, Basic) {
  Cartesian point;
  MutableRepresentationView view(point);

  EXPECT_THAT(Vec3(point), Pointwise(DoubleNear(1e-12), Vec3(0, 0, 0)));

  view = Cartesian(1, 2, 3);

  EXPECT_THAT(Vec3(point), Pointwise(DoubleNear(1e-12), Vec3(1, 2, 3)));

  EXPECT_NEAR(view.cartesian().x, 1.0, 1e-12);
  EXPECT_NEAR(view.cartesian().y, 2.0, 1e-12);
  EXPECT_NEAR(view.cartesian().z, 3.0, 1e-12);
}

TEST(MutableRepresentationView, FromCartesian) {
  Cartesian point;
  MutableRepresentationView view(point);

  view = Cartesian(1, 2, 3);

  const Cartesian cartesian = view.cartesian();

  EXPECT_NEAR(cartesian.x, 1.0, 1e-12);
  EXPECT_NEAR(cartesian.y, 2.0, 1e-12);
  EXPECT_NEAR(cartesian.z, 3.0, 1e-12);
}

TEST(MutableRepresentationView, FromSpherical) {
  Cartesian point;
  MutableRepresentationView view(point);

  view = Spherical({.latitude = 0.1, .longitude = 0.2, .distance = 0.3});

  const Spherical spherical = view.spherical();

  EXPECT_NEAR(spherical.latitude, 0.1, 1e-6);
  EXPECT_NEAR(spherical.longitude, 0.2, 1e-6);
  EXPECT_NEAR(spherical.distance, 0.3, 1e-6);
}

TEST(MutableRepresentationView, Optional) {
  std::optional<Cartesian> point;
  MutableRepresentationView view(point);

  EXPECT_FALSE(point.has_value());

  view = Cartesian(1, 2, 3);

  EXPECT_TRUE(point.has_value());

  EXPECT_THAT(Vec3(*point), Pointwise(DoubleNear(1e-12), Vec3(1, 2, 3)));

  EXPECT_NEAR(view.cartesian().x, 1.0, 1e-12);
  EXPECT_NEAR(view.cartesian().y, 2.0, 1e-12);
  EXPECT_NEAR(view.cartesian().z, 3.0, 1e-12);
}

}  // namespace astro_core
