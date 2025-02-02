// Copyright (c) 2024 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/internal/nutation_arguments.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

TEST(earth, CalculateLunisolarNutationArguments) {
  // Obtain the t:
  //   >>> from astropy.time import Time
  //   >>> time = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  //   >>> t = (time.tt.jd1 - 2451545 + time.tt.jd2)/36525
  //   >>> t
  //   0.21767284875402437

  const double t = 0.21767284875402437;

  const LunisolarNutationArguments args =
      CalculateLunisolarNutationArguments(t);

  // Mean Anomaly of the Moon, eraFal03().
  // >>> import erfa
  // >>> erfa.fal03(0.21767284875402437)
  // 5.728183630872013
  EXPECT_NEAR(args.l, 5.728183630872013, 1e-12);

  // l', Mean Anomaly of the Sun, eraFalp03().
  // >>> import erfa
  // >>> erfa.falp03(0.21767284875402437)
  // 4.774259701773741
  EXPECT_NEAR(args.l_prime, 4.774259701773741, 1e-12);

  // Mean longitude of the Moon minus that of the ascending node, eraFaf03().
  // >>> import erfa
  // >>> erfa.faf03(0.21767284875402437)
  // 2.674395702585702
  EXPECT_NEAR(args.F, 2.674395702585702, 1e-12);

  // Mean Elongation of the Moon from the Sun, eraFad03().
  // >>> import erfa
  // >>> erfa.fad03(0.21767284875402437)
  // 0.35623421040122616
  EXPECT_NEAR(args.D, 0.35623421040122616, 1e-12);

  // Mean Longitude of the Ascending Node of the Moon, eraFaom03().
  // >>> import erfa
  // >>> erfa.faom03(0.21767284875402437)
  // -5.16555144479436
  EXPECT_NEAR(args.Om, -5.16555144479436, 1e-12);
}

TEST(earth, CalculatePlanetaryNutationArguments) {
  // Obtain the t:
  //   >>> import erfa
  //   >>> from astropy.time import Time
  //   >>> time = Time('2021-10-08T00:00:00.000', format='isot', scale='utc')
  //   >>> t = (time.tt.jd1 - 2451545 + time.tt.jd2)/36525
  //   >>> t
  //   0.21767284875402437

  const double t = 0.21767284875402437;

  const PlanetaryNutationArguments args =
      CalculatePlanetaryNutationArguments(t);

  // Mercury, eraFame03().
  // >>> import erfa
  // >>> erfa.fame03(0.21767284875402437)
  // 0.49556537320517435
  EXPECT_NEAR(args.L_Me, 0.49556537320517435, 1e-12);

  // Venus, eraFave03().
  // >>> import erfa
  // >>> erfa.fave03(0.21767284875402437)
  // 5.580156943919512
  EXPECT_NEAR(args.L_Ve, 5.580156943919512, 1e-12);

  // Earth, eraFae03().
  // >>> import erfa
  // >>> erfa.fae03(0.21767284875402437)
  // 0.2888954765644911
  EXPECT_NEAR(args.L_E, 0.2888954765644911, 1e-12);

  // Mars, eraFama03().
  // >>> import erfa
  // >>> erfa.fama03(0.21767284875402437)
  // 3.521319577133312
  EXPECT_NEAR(args.L_Ma, 3.521319577133312, 1e-12);

  // Jupiter, eraFaju03().
  // >>> import erfa
  // >>> erfa.faju03(0.21767284875402437)
  // 5.846295269553211
  EXPECT_NEAR(args.L_J, 5.846295269553211, 1e-12);

  // Saturn, eraFasa03().
  // >>> import erfa
  // >>> erfa.fasa03(0.21767284875402437)
  // 5.516959138332685
  EXPECT_NEAR(args.L_Sa, 5.516959138332685, 1e-12);

  // Uranus, eraFaur03().
  // >>> import erfa
  // >>> erfa.faur03(0.21767284875402437)
  // 0.8259009242662891
  EXPECT_NEAR(args.L_U, 0.8259009242662891, 1e-12);

  // Neptune, eraFane03().
  // >>> import erfa
  // >>> erfa.fane03(0.21767284875402437)
  // 6.14193893689622
  EXPECT_NEAR(args.L_Ne, 6.14193893689622, 1e-12);

  // General accumulated precession in longitude., eraFapa03().
  // >>> import erfa
  // >>> erfa.fapa03(0.21767284875402437)
  // 0.005307500219818061
  EXPECT_NEAR(args.p_A, 0.005307500219818061, 1e-12);
}

}  // namespace astro_core
