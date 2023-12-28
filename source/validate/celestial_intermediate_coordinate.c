// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT-0

// A code which uses ERFA library to generate verification data for the
// Celestial Intermediate Pole coordinate X,Y and Celestial Intermediate Origin
// locator s'.
//
// clang-format off
//   $ ERFA=/opt/local/erfa make celestial_intermediate_coordinate
//   $ ./bin/celestial_intermediate_coordinate
//   XY = (0.00217525068235324128482210959135 0.00002453560612521409124470346541)
//   s = -0.00000003575303203556068598344649
//   rc2i = (
//     (0.99999763413943376644255067731137, 0.00000000906745347082822503316740, -0.00217525068323046232368622732167),
//     (-0.00000006243852600506019657740451, 0.99999999969900199570105314705870, -0.00002453552835340674248033876470),
//     (0.00217525068235324128482210959135, 0.00002453560612521409124470346541, 0.99999763383843709441123337455792))
// clang-format on

#include <erfa.h>
#include <stdio.h>

int main() {
  const double date_tt_1 = 2459802.0;
  const double date_tt_2 = 0.4174674074074074;

  double x, y;
  eraXy06(date_tt_1, date_tt_2, &x, &y);

  const double s = eraS06(date_tt_1, date_tt_2, x, y);

  double rc2i[3][3];
  eraC2ixys(x, y, s, rc2i);

  printf("XY = (%.32f %.32f)\n", x, y);
  printf("s = %.32f\n", s);

  printf(
      "rc2i = (\n"
      "  (%.32f, %.32f, %.32f),\n"
      "  (%.32f, %.32f, %.32f),\n"
      "  (%.32f, %.32f, %.32f))\n",
      rc2i[0][0],
      rc2i[0][1],
      rc2i[0][2],
      rc2i[1][0],
      rc2i[1][1],
      rc2i[1][2],
      rc2i[2][0],
      rc2i[2][1],
      rc2i[2][2]);

  return 0;
}
