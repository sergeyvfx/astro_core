// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT-0

// A code which uses ERFA library to convert position and velocity from
// cartesian representation to spherical.
// Used to generate test data for CartesianDifferential::ToSpherical().
//
// clang-format off
//   $ ERFA=/opt/local/erfa make representation_cartesian_to_spherical
//   $ ./bin/representation_cartesian_to_spherical
//   Position:
//     lon=1.10714871779409040897235172451474
//     lat=0.93027401411547205878349586782861
//     r=3.74165738677394132949416416522581
//   Velocity:
//     d_lon=-0.06000000000000001165734175856414
//     d_lat=-0.03833259389999636723711518015989
//     d_r=0.85523597411975793569638426561141
// clang-format on

#include <erfa.h>
#include <stdio.h>

int main() {
  double pv[2][3] = {{1.0, 2.0, 3.0}, {0.4, 0.5, 0.6}};
  double theta, phi, r;  // longitude, latitude, distance.
  double td, pd, rd;     // Rate of changes.

  eraPv2s(pv, &theta, &phi, &r, &td, &pd, &rd);

  printf("Position:\n  lon=%.32f\n  lat=%.32f\n  r=%.32f\n", theta, phi, r);
  printf("Velocity:\n  d_lon=%.32f\n  d_lat=%.32f\n  d_r=%.32f\n", td, pd, rd);

  return 0;
}
