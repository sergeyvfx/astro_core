// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/orientation_table.h"

#include "astro_core/earth/internal/orientation_test_data.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::DoubleNear;
using testing::Pointwise;

TEST(EarthOrientationTable, LookupUT1MinusUTCSecondsInUTCScale) {
  const EarthOrientationTable table = test_data::CreateEarthOrientationTable();

  // The reference values has been obtained using Astropy 5.3.4.

  // MJD is prior the first table row.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.iers_degraded_accuracy = "warn"
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.ut1_utc(Time(30000, format='mjd', scale='utc')).value
  //   0.0326338
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(30000)),
      0.0326338,
      1e-12);

  // MJD is at the table row.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.ut1_utc(Time(50815, format='mjd', scale='utc')).value
  //   0.2165438
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(50815)),
      0.2165438,
      1e-12);

  // MJD is in-between of 2 table rows.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.ut1_utc(Time(50815.4, format='mjd', scale='utc')).value
  //   0.21584087999999743
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(50815.4)),
      0.21584087999999743,
      1e-9);

  // MJD is past the last table row.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.iers_degraded_accuracy = "warn"
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.ut1_utc(Time(100000, format='mjd', scale='utc')).value
  //   0.0028596
  //
  // NOTE: The Astropy's eopc04.1962-now bulletin is slightly older than the
  // one in astro_core at th moment of writing this test. Hence the difference
  // in the value.
  EXPECT_NEAR(
      table.LookupUT1MinusUTCSecondsInUTCScale(ModifiedJulianDate(100000)),
      0.0043809,
      1e-12);
}

TEST(EarthOrientationTable, LookupPolarMotionArcsecInUTCScale) {
  // The reference values has been obtained using Astropy 5.3.4.

  const EarthOrientationTable table = test_data::CreateEarthOrientationTable();

  // MJD is prior the first table row.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.iers_degraded_accuracy = "warn"
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.pm_xy(Time(30000, format='mjd', scale='utc'))
  //   [<Quantity -0.0127 arcsec>, <Quantity 0.213 arcsec>]
  EXPECT_THAT(
      table.LookupPolarMotionArcsecInUTCScale(ModifiedJulianDate(30000)),
      Pointwise(DoubleNear(1e-12), {-0.0127, 0.213}));

  // MJD is at the table row.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.pm_xy(Time(50815, format='mjd', scale='utc'))
  //   [<Quantity 0.099996 arcsec>, <Quantity 0.173811 arcsec>]
  EXPECT_THAT(
      table.LookupPolarMotionArcsecInUTCScale(ModifiedJulianDate(50815)),
      Pointwise(DoubleNear(1e-12), {0.099996, 0.173811}));

  // MJD is in-between of 2 table rows.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.pm_xy(Time(50815.4, format='mjd', scale='utc'))
  //   [<Quantity 0.0989724 arcsec>, <Quantity 0.1734982 arcsec>]
  EXPECT_THAT(
      table.LookupPolarMotionArcsecInUTCScale(ModifiedJulianDate(50815.4)),
      Pointwise(DoubleNear(1e-12), {0.0989724, 0.1734982}));

  // MJD is past the last table row.
  //
  //   >>> from astropy.time import Time
  //   >>> from astropy.utils import iers
  //   >>> iers.conf.iers_degraded_accuracy = "warn"
  //   >>> from astropy.utils import iers
  //   >>> iers_b = iers.IERS_B.open(iers.IERS_B_FILE)
  //   >>> iers_b.pm_xy(Time(100000, format='mjd', scale='utc'))
  //   [<Quantity 0.304481 arcsec>, <Quantity 0.414958 arcsec>]
  //
  // NOTE: The Astropy's eopc04.1962-now bulletin is slightly older than the
  // one in astro_core at th moment of writing this test. Hence the difference
  // in the value.
  EXPECT_THAT(
      table.LookupPolarMotionArcsecInUTCScale(ModifiedJulianDate(100000)),
      Pointwise(DoubleNear(1e-12), {0.305032, 0.395644}));
}

}  // namespace astro_core
