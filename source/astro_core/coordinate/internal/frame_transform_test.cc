// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/coordinate/frame_transform.h"

#include "astro_core/earth/internal/earth_test_data.h"
#include "astro_core/time/format/date_time.h"
#include "astro_core/time/time.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

class FrameTransformTest : public testing::Test {
 protected:
  void SetUp() override { test_data::SetTables(); }
};

////////////////////////////////////////////////////////////////////////////////
// TEME<->ITRF tests.
//
// The result is verified using transform_teme_to_itrf.c which mimics the code
// from Astropy, but it has different precision. Possibly the fact that Astropy
// does some round-trips between CPython and C ERFA adds some rounding errors.
//
// NOTE: The values given are in kilometers, although the internal format is
// meters.

TEST_F(FrameTransformTest, TEMEToPEFMatrix) {
  const Vec3 r_teme{4357.092619856639, 4500.439126822302, -2645.108425391841};

  const Mat3 teme_to_pef =
      TEMEToPEFMatrix(Time(DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC));

  const Vec3 r_pef = teme_to_pef * r_teme;

  // The reference value is obtained by forcing xp and yp variables to 0 in the
  // test code from above.
  EXPECT_THAT(r_pef,
              Pointwise(DoubleNear(1e-12),
                        {-2801.424431376662596449,
                         5602.698420183636699221,
                         -2645.108425391841137753}));
}

TEST_F(FrameTransformTest, PEFToITRFMatrix) {
  const Vec3 r_pef{4357.092619856639, 4500.439126822302, -2645.108425391841};

  const Mat3 pef_to_itrf =
      PEFToITRFMatrix(Time(DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC));

  const Vec3 r_itrf = pef_to_itrf * r_pef;

  // The reference value is obtained by forcing gst variable to 0 in the test
  // code from above.
  EXPECT_THAT(r_itrf,
              Pointwise(DoubleNear(1e-12),
                        {4357.088844427065851050,
                         4500.444007597445306601,
                         -2645.106340125221322523}));
}

TEST_F(FrameTransformTest, TEMEToITRFMatrix) {
  // The Python code which performs the same conversion. Astropy version 5.3.4
  // vas used. The accuracy seems to be different from the code which directly
  // uses ERFA.
  // The ERFA-based "TEME to ITRF chain" code above has been used to obtain
  // expected values.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> from astropy.coordinates import ITRS, TEME
  //   >>> from astropy.coordinates import CartesianRepresentation
  //   >>> from astropy import units as u
  //   >>> t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")
  //   >>> r_teme = CartesianRepresentation(
  //   ...     [4357.092619856639, 4500.439126822302, -2645.108425391841]*u.km)
  //   >>> teme = TEME(r_teme, obstime=t)
  //   >>> itrs = teme.transform_to(ITRS(obstime=t))
  //   >>> location = itrs.earth_location
  //   >>> location.x.value, location.y.value, location.z.value
  //   (-2801.428206798785, 5602.70330093813, -2645.0940887103257)

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_teme{4357.092619856639, 4500.439126822302, -2645.108425391841};

  const Mat3 teme_to_itrf = TEMEToITRFMatrix(time);
  const Vec3 r_itrf = teme_to_itrf * r_teme;

  EXPECT_THAT(r_itrf,
              Pointwise(DoubleNear(1e-12),
                        {-2801.428206798944302136,
                         5602.703300938050233526,
                         -2645.094088710325195279}));
}

TEST_F(FrameTransformTest, TEMEToITRF) {
  // The exact value is based on the state of TEMEToITRF which produces the
  // verified position transformation using the "TEME to ITRF chain" in the
  // beginning of this test suit file.
  //
  // The Python to approximate the value. Astropy version 5.3.4 was used.
  // Note that similarly to the coordinate transformation case the precision in
  // the Python code is different from what the naive implementation in C++ has.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> from astropy.coordinates import ITRS, TEME
  //   >>> from astropy.coordinates import CartesianRepresentation
  //   >>> from astropy.coordinates import CartesianDifferential
  //   >>> from astropy import units as u
  //   >>> t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")
  //   >>> r_teme = CartesianRepresentation(
  //   ...     [4357.092619856639, 4500.439126822302, -2645.108425391841]*u.km)
  //   >>> v_teme = CartesianDifferential(
  //   ...     [-2.1768117558889037, 5.163121595591936, 5.215977759982141]*
  //   ...     u.km/u.s)
  //   >>> teme = TEME(r_teme.with_differentials(v_teme), obstime=t)
  //   >>> itrs = teme.transform_to(ITRS(obstime=t))
  //   >>> itrs.x.value, itrs.y.value, itrs.z.value
  //   (-2801.428206798785, 5602.70330093813, -2645.0940887103257)
  //   >>> itrs.v_x.value, itrs.v_y.value, itrs.v_z.value
  //   (-5.184234303148969, -0.13771424946298794, 5.215984904648849)

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_teme{4357.092619856639, 4500.439126822302, -2645.108425391841};
  const Vec3 v_teme{-2.1768117558889037, 5.163121595591936, 5.215977759982141};

  Vec3 r_itrf, v_itrf;
  TEMEToITRF(time, r_teme, v_teme, r_itrf, v_itrf);

  EXPECT_THAT(r_itrf,
              Pointwise(DoubleNear(1e-12),
                        {-2801.428206798944302136,
                         5602.703300938050233526,
                         -2645.094088710325195279}));

  EXPECT_THAT(v_itrf,
              Pointwise(DoubleNear(1e-12),
                        {-5.184234346857372167,
                         -0.137714270932494498,
                         5.215984905464025267}));
}

TEST_F(FrameTransformTest, ITRFToTEME) {
  // The test data is the reverse of the test of TEMEToITRF().
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.auto_download = False
  //   >>> from astropy.coordinates import ITRS, TEME
  //   >>> from astropy.coordinates import CartesianRepresentation
  //   >>> from astropy.coordinates import CartesianDifferential
  //   >>> from astropy import units as u
  //   >>> t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")
  //   >>> r_itrf = CartesianRepresentation(
  //   ...     [-2801.428206798944302136,
  //   ...      5602.703300938050233526,
  //   ...      -2645.094088710325195279] * u.km)
  //   >>> v_itrf = CartesianDifferential(
  //   ...     [-5.184234346857372167,
  //   ...      -0.137714270932494498,
  //   ...      5.215984905464025267] * u.km / u.s)
  //   >>> itrs = ITRS(r_itrf.with_differentials(v_itrf), obstime=t)
  //   >>> teme = itrs.transform_to(TEME(obstime=t))
  //   >>> teme.x.value, teme.y.value, teme.z.value
  //   (4357.092619856512, 4500.439126822426, -2645.108425391841)

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};
  const Vec3 r_itrf{-2801.428206798944302136,
                    5602.703300938050233526,
                    -2645.094088710325195279};
  const Vec3 v_itrf{
      -5.184234346857372167, -0.137714270932494498, 5.215984905464025267};

  Vec3 r_teme, v_teme;
  ITRFToTEME(time, r_itrf, v_itrf, r_teme, v_teme);

  EXPECT_THAT(
      r_teme,
      Pointwise(DoubleNear(1e-12),
                {4357.092619856639, 4500.439126822302, -2645.108425391841}));

  EXPECT_THAT(
      v_teme,
      Pointwise(DoubleNear(1e-12),
                {-2.1768117558889037, 5.163121595591936, 5.215977759982141}));
}

////////////////////////////////////////////////////////////////////////////////
// ITRF<->GCRF tests.
//
// The result is verified using transform_itrf_to_gcrf.c which mimics the code
// from Astropy, but it has different precision. Possibly the fact that Astropy
// does some round-trips between CPython and C ERFA adds some rounding errors.
//
// NOTE: The values given are in kilometers, although the internal format is
// meters.

TEST_F(FrameTransformTest, ITRFToGCRF) {
  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};

  const Vec3 r_itrf(-2801.428206798944302136,
                    5602.703300938050233526,
                    -2645.094088710325195279);
  const Vec3 v_itrf(
      -5.184234346857372167, -0.137714270932494498, 5.215984905464025267);

  Vec3 r_gcrf, v_gcrf;

  ITRFToGCRF(time, r_itrf, v_itrf, r_gcrf, v_gcrf);

  EXPECT_THAT(r_gcrf,
              Pointwise(DoubleNear(1e-12),
                        {4374.025673658524283383,
                         4478.288319286147270759,
                         -2654.739186783237528289}));

  EXPECT_THAT(
      v_gcrf,
      Pointwise(
          DoubleNear(1e-12),
          {-2.139329590299860584, 5.174189009638810788, 5.220516738855706329}));
}

TEST_F(FrameTransformTest, GCRFToITRF) {
  // The test data is the reverse of the test of ITRFToGCRF().

  const Time time{DateTime(2022, 8, 10, 22, 0, 0), TimeScale::kUTC};

  const Vec3 r_gcrf(4374.025673658524283383,
                    4478.288319286147270759,
                    -2654.739186783237528289);
  const Vec3 v_gcrf(
      -2.139329590299860584, 5.174189009638810788, 5.220516738855706329);

  Vec3 r_itrf, v_itrf;

  GCRFToITRF(time, r_gcrf, v_gcrf, r_itrf, v_itrf);

  EXPECT_THAT(r_itrf,
              Pointwise(DoubleNear(1e-12),
                        {-2801.428206798944302136,
                         5602.703300938050233526,
                         -2645.094088710325195279}));

  EXPECT_THAT(v_itrf,
              Pointwise(DoubleNear(1e-12),
                        {-5.184234346857372167,
                         -0.137714270932494498,
                         5.215984905464025267}));
}

}  // namespace astro_core
