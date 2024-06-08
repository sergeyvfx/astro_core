// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/frame.h"

#include <utility>

#include "astro_core/coordinate/cartesian.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(PositionFrame, CopyAndAssign) {
  using Frame = PositionFrame<Cartesian>;

  // Copy constructor.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
    });
    Frame frame(src);

    src.position = Cartesian(4, 5, 6);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
  }

  // Move constructor.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
    });
    Frame frame(std::move(src));

    src.position = Cartesian(4, 5, 6);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
  }

  // Copy operator.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
    });
    Frame frame;

    // Some extra code to avoid optimizer to replace assignment with copy
    // constructor.
    frame.observation_time = {};

    frame = src;

    src.position = Cartesian(4, 5, 6);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
  }

  // Move operator.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
    });
    Frame frame;

    // Some extra code to avoid optimizer to replace assignment with copy
    // constructor.
    frame.observation_time = {};

    frame = std::move(src);

    src.position = Cartesian(4, 5, 6);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
  }
}

TEST(PositionVelocityFrame, CopyAndAssign) {
  using Frame = PositionVelocityFrame<Cartesian, CartesianDifferential>;

  // Copy constructor.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
        .velocity = CartesianDifferential(4, 5, 6),
    });
    Frame frame(src);

    src.position = Cartesian(7, 8, 9);
    src.velocity = CartesianDifferential(10, 11, 12);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
    EXPECT_EQ(frame.velocity.GetCartesian(), Vec3(4, 5, 6));
  }

  // Move constructor.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
        .velocity = CartesianDifferential(4, 5, 6),
    });
    Frame frame(std::move(src));

    src.position = Cartesian(7, 8, 9);
    src.velocity = CartesianDifferential(10, 11, 12);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
    EXPECT_EQ(frame.velocity.GetCartesian(), Vec3(4, 5, 6));
  }

  // Copy operator.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
        .velocity = CartesianDifferential(4, 5, 6),
    });
    Frame frame;

    // Some extra code to avoid optimizer to replace assignment with copy
    // constructor.
    frame.observation_time = {};

    frame = src;

    src.position = Cartesian(7, 8, 9);
    src.velocity = CartesianDifferential(10, 11, 12);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
    EXPECT_EQ(frame.velocity.GetCartesian(), Vec3(4, 5, 6));
  }

  // Move operator.
  {
    Frame src({
        .observation_time = {},
        .position = Cartesian(1, 2, 3),
        .velocity = CartesianDifferential(4, 5, 6),
    });
    Frame frame;

    // Some extra code to avoid optimizer to replace assignment with copy
    // constructor.
    frame.observation_time = {};

    frame = std::move(src);

    src.position = Cartesian(7, 8, 9);
    src.velocity = CartesianDifferential(10, 11, 12);

    EXPECT_EQ(frame.position.GetCartesian(), Vec3(1, 2, 3));
    EXPECT_EQ(frame.velocity.GetCartesian(), Vec3(4, 5, 6));
  }
}

}  // namespace astro_core
