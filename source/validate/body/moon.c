// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT-0

// A code which uses ERFA library to generate verification data for the
// Moon coordinates in moon_test.cc.
//
// clang-format off
//   $ ERFA=/opt/local/erfa make body_moon
//   $ ./bin/body_moon
//   p: (-0.001685642035962147, 0.001697465227392170, 0.000584745681294692) AU
//      (-252168459.342350006103515625, 253937183.605159878730773926, 87476708.822706788778305054) m
//   v: (-0.000440034036330798, -0.000375144412394738, -0.000207616547668232)
// clang-format on

#include <erfa.h>
#include <erfam.h>
#include <stdio.h>

int main() {
  const double jd1 = 2448725.0;
  const double jd2 = -0.4993265740740741;

  double pv[2][3];
  eraMoon98(jd1, jd2, pv);

  printf("p: (%.18f, %.18f, %.18f) AU\n", pv[0][0], pv[0][1], pv[0][2]);
  printf("   (%.18f, %.18f, %.18f) m\n",
         pv[0][0] * ERFA_DAU,
         pv[0][1] * ERFA_DAU,
         pv[0][2] * ERFA_DAU);

  printf("v: (%.18f, %.18f, %.18f)\n", pv[1][0], pv[1][1], pv[1][2]);

  return 0;
}
