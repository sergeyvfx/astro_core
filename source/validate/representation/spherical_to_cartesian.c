// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT-0

// A code which uses ERFA library to convert position and velocity from
// spherical representation to cartesian.
// Used to generate test data for SphericalDifferential::ToSpherical().
//
// clang-format off
//   $ ERFA=/opt/local/erfa make representation_spherical_to_cartesian
//   $ ./bin/representation_spherical_to_cartesian
//   Position: (-0.67453528609845869734584766774788, 1.47388648930164567474321302142926, 2.52441295442368929258236676105298)
//   Velocity: (-0.19919842037932808387346028666798, -1.12275791843049321805381168815074, 1.31533604968694750603219745244132)
// clang-format on

#include <erfa.h>
#include <stdio.h>

int main() {
  double theta = 2.0, phi = 1.0, r = 3.0;  // longitude, latitude, distance.
  double td = 0.4, pd = 0.5, rd = 0.6;     // Rate of changes.
  double pv[2][3];

  eraS2pv(theta, phi, r, td, pd, rd, pv);

  printf("Position: (%.32f, %.32f, %.32f)\n", pv[0][0], pv[0][1], pv[0][2]);
  printf("Velocity: (%.32f, %.32f, %.32f)\n", pv[1][0], pv[1][1], pv[1][2]);

  return 0;
}
