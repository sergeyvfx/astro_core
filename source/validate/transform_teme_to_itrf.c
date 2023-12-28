// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT-0

// A code which uses ERFA library to generate verification data for the
// TEME to ITRF transformation in transform_test.cc.
//
// clang-format off
//   $ ERFA=/opt/local/erfa make transform_teme_to_itrf
//   $ ./bin/transform_teme_to_itrf
//   r_itrf: -2801.428206798944302136 5602.703300938050233526 -2645.094088710325195279
// clang-format on

#include <erfa.h>
#include <stdio.h>

int main() {
  // Obtained from the astro_core library to get better precision and
  // verified against the Astropy library.
  //
  // astro_core code:
  //   std::cout << std::setprecision(33)
  //             << double(GreenwichMeanSiderealTime1982(
  //                    Time{DateTime(2022, 8, 10, 22, 0, 0),
  //                         TimeScale::kUTC}
  //                        .ToScale<TimeScale::kUT1>()
  //                        .AsFormat<JulianDate>()))
  //             << std::endl;
  //   std::cout << std::setprecision(33)
  //             << GetEarthPolarMotionInUTCScale(
  //                    Time{DateTime(2022, 8, 10, 22, 0, 0),
  //                         TimeScale::kUTC}
  //                        .ToScale<TimeScale::kUTC>()
  //                        .AsFormat<ModifiedJulianDate>())(1)
  //             << std::endl;
  //
  // Astropy version 5.3.4 was used.
  //
  //   >>> import erfa
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> from astropy.coordinates.builtin_frames.utils import (
  //   ...     get_polar_motion)
  //   >>> t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")
  //   >>> erfa.gmst82(t.ut1.jd1, t.ut1.jd2)
  //   5.050310953943878
  //   >>> get_polar_motion(t)
  //   (1.427323394087147e-06, 1.8452065264625068e-06)

  const double gst = 5.05031095394384976771107176318765;
  const double xp = 0.00000142732339408714701315631246530957;
  const double yp = 0.0000018452065264625071883570215161563;

  double r_teme[3] = {4357.092619856639, 4500.439126822302, -2645.108425391841};

  double identity[3][3];
  eraIr(identity);

  double pmmat[3][3];
  eraPom00(xp, yp, 0, pmmat);

  double m[3][3];
  eraC2tcio(identity, gst, pmmat, m);

  double r_itrf[3];
  eraRxp(m, r_teme, r_itrf);

  printf("r_itrf: %.18f %.18f %.18f\n", r_itrf[0], r_itrf[1], r_itrf[2]);

  return 0;
}
