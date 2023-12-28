// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT-0

// A code which uses ERFA library to generate verification data for the
// ITRF to GCRF transformation in transform_test.cc.
//
// clang-format off
//   $ ERFA=/opt/local/erfa make transform_itrf_to_gcrf
//   $ ./bin/transform_itrf_to_gcrf
//   r_gcrf: (4374.025673658524283383, 4478.288319286147270759, -2654.739186783237528289)
//   v_gcrf: (-2.139329590299860584, 5.174189009638810788, 5.220516738855706329)
// clang-format on
//
// This is an implementation of Method (1) of the ITRF<->GCRF algorithm
// implementation described in [IERS2010] Page 71.
//
// References:
//
//   [IERS2010] Gerard Petit, and Brian Luzum, IERS Conventions (2010).
//
//   [Vallado2013] David A Vallado and Wayne D Macclain. 2013.
//       Fundamentals of astrodynamics and applications.

#include <erfa.h>
#include <stdio.h>

int main() {
  // Obtained from the astro_core library to get better precision and
  // verified against the Astropy library.
  //
  // astro_core code:
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
  //   >>> t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")
  //   >>> t.tt.jd1, t.tt.jd2
  //   (2459802.0, 0.4174674074074074)
  //   >>> t.ut1.jd1, t.ut1.jd2
  //   (2459802.0, 0.4166663200011574)

  const double xp = 0.00000142732339408714701315631246530957;
  const double yp = 0.0000018452065264625071883570215161563;

  const double date_tt_1 = 2459802.0;
  const double date_tt_2 = 0.4174674074074074;

  const double date_ut1_1 = 2459802.0;
  const double date_ut1_2 = 0.4166663200011574;

  // Construct ITRF-to-GCRF matrix.
  // Algorithm is the Method (1) from [IERS2010] Page 71.

  double X, Y;
  eraXy06(date_tt_1, date_tt_2, &X, &Y);

  const double s = eraS06(date_tt_1, date_tt_2, X, Y);

  double gcrf_to_cirs[3][3];
  eraC2ixys(X, Y, s, gcrf_to_cirs);

  const double era = eraEra00(date_ut1_1, date_ut1_2);

  double cirs_to_tirs[3][3];
  eraIr(cirs_to_tirs);
  eraRz(era, cirs_to_tirs);

  const double sp = eraSp00(date_tt_1, date_tt_2);

  double tirs_to_itrf[3][3];
  eraPom00(xp, yp, sp, tirs_to_itrf);

  double gcrf_to_tirs[3][3];
  eraRxr(cirs_to_tirs, gcrf_to_cirs, gcrf_to_tirs);

  double gcrf_to_itrf[3][3];
  eraRxr(tirs_to_itrf, gcrf_to_tirs, gcrf_to_itrf);

  double itrf_to_gcrf[3][3];
  eraTr(gcrf_to_itrf, itrf_to_gcrf);

  // Perform position vector conversion.

  /*const*/ double r_itrf[3] = {-2801.428206798944302136,
                                5602.703300938050233526,
                                -2645.094088710325195279};

  double r_gcrf[3];
  eraRxp(itrf_to_gcrf, r_itrf, r_gcrf);

  printf("r_gcrf: (%.18f, %.18f, %.18f)\n", r_gcrf[0], r_gcrf[1], r_gcrf[2]);

  // Perform transformation of velocity.
  // Follows equations from [Vallado2013] Page 220.

  // Inverse the individual matrices and give them the same naming as in the
  // [Vallado2013].
  double PN[3][3];
  eraTr(gcrf_to_cirs, PN);

  double R[3][3];
  eraTr(cirs_to_tirs, R);

  double W[3][3];
  eraTr(tirs_to_itrf, W);

  double v_itrf[3] = {
      -5.184234346857372167, -0.137714270932494498, 5.215984905464025267};

  double omega[3] = {0.0, 0.0, 7.292115146706979e-5};

  double itrf_to_tirs[3][3];
  eraTr(tirs_to_itrf, itrf_to_tirs);

  double r_tirs[3];
  eraRxp(itrf_to_tirs, r_itrf, r_tirs);

  double w_x_r_tirs[3];
  eraPxp(omega, r_tirs, w_x_r_tirs);

  double W_x_v_itrf[3];
  eraRxp(W, v_itrf, W_x_v_itrf);

  double v_gcrf[3];
  eraPpp(W_x_v_itrf, w_x_r_tirs, v_gcrf);
  eraRxp(R, v_gcrf, v_gcrf);
  eraRxp(PN, v_gcrf, v_gcrf);

  printf("v_gcrf: (%.18f, %.18f, %.18f)\n", v_gcrf[0], v_gcrf[1], v_gcrf[2]);

  return 0;
}
