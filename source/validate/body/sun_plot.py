#!/usr/bin/env python3

"""
A verification of the Sun position prediction.

This code implements an algorithm described in the [almanac08] with
modifications from [usno] to expand the accurate range to years 1800 to 2200.
It verifies the implementation against Astropy's `astropy.coordinates.get_sun()`
and plots the difference. The expected difference is within 60 arcseconds.

References:

  [almanac08] U.S. Naval Observatory; U.K. Hydrographic Office, H.M. Nautical
    Almanac Office (2008). The Astronomical Almanac for the Year 2010. U.S.
    Govt. Printing Office. p. C5. ISBN 978-0-7077-4082-9.

  [usno] U.S. Naval Observatory, Approximate Solar Coordinates.
    https://aa.usno.navy.mil/faq/sun_approx
    https://web.archive.org/web/20181115153648/http://aa.usno.navy.mil/faq/docs/SunApprox.php
"""

import astropy.coordinates
import erfa
import math
import matplotlib.pyplot as plt
import numpy as np
import sys
import unittest

from astropy import units as u
from astropy.time import Time, TimeDelta
from multiprocessing.pool import Pool


def wrap_angle(angle: float) -> float:
    """
    Wrap angle denoted in radians to the [0 .. 2*pi) range.
    """
    return angle % (2 * math.pi)


class WrapAngleTest(unittest.TestCase):
    """
    Unit test for the wrap_angle()
    """

    def test_basic(self) -> None:
        pi = math.pi

        self.assertAlmostEqual(wrap_angle(0), 0)
        self.assertAlmostEqual(wrap_angle(pi / 6), pi / 6)
        self.assertAlmostEqual(wrap_angle(pi - pi / 6), pi - pi / 6)
        self.assertAlmostEqual(wrap_angle(pi + pi / 6), pi + pi / 6)
        self.assertAlmostEqual(wrap_angle(pi), pi)

    def test_wrap_positive(self) -> None:
        pi = math.pi

        self.assertAlmostEqual(wrap_angle(2 * pi + pi / 6), pi / 6)

    def test_wrap_negative(self) -> None:
        pi = math.pi

        self.assertAlmostEqual(wrap_angle(-pi / 6), 2 * pi - pi / 6)
        self.assertAlmostEqual(wrap_angle(-pi), pi)


def angle_delta(a: float, b: float) -> float:
    """
    A difference between two angles denoted in radians.

    The difference is within [-pi, pi].
    The difference is such: a = b + delta
    """
    pi = math.pi
    a = wrap_angle(a)
    b = wrap_angle(b)
    return (a - b + pi) % (2 * pi) - pi


class AngleDeltaTest(unittest.TestCase):
    """
    Unit test for the angle_delta()
    """

    def test_basic(self) -> None:
        pi = math.pi

        self.assertAlmostEqual(angle_delta(0, pi / 6), -pi / 6)
        self.assertAlmostEqual(angle_delta(pi / 6, 0), pi / 6)

    def test_wrap(self) -> None:
        pi = math.pi

        self.assertAlmostEqual(angle_delta(pi / 6, 11 * pi / 6), pi / 3)
        self.assertAlmostEqual(angle_delta(11 * pi / 6, pi / 6), -pi / 3)


def degrees_to_radians(value: float) -> float:
    """
    Convert values given in degrees to radians
    """
    return value * math.pi / 180.0


class DegreesToRadiansTest(unittest.TestCase):
    """
    Unit test for the degrees_to_radians()
    """

    def test_basic(self) -> None:
        self.assertAlmostEqual(degrees_to_radians(60), 1.0471975511965976)


def radians_to_arcsec(value: float) -> float:
    """
    Convert value measured in radians to value measured arc seconds.
    """
    return value * (180.0 * 3600.0) / math.pi


class RadiansToArcsecTest(unittest.TestCase):
    """
    Unit test for the radians_to_arcsec()
    """

    def test_basic(self) -> None:
        self.assertAlmostEqual(radians_to_arcsec(0.0001454441043328608), 30)


def arcsec_to_radians(value: float) -> float:
    """
    Convert value measured in arc seconds to value measured in radians.
    """
    return value * math.pi / (180.0 * 3600.0)


class ArcsecToRadiansTest(unittest.TestCase):
    """
    Unit test for the arcsec_to_radians()
    """

    def test_basic(self) -> None:
        self.assertAlmostEqual(arcsec_to_radians(30), 0.0001454441043328608)


class SunPosition:
    """
    Position of a sun in the spherical coordinates.
    """

    # Right ascension, degrees.
    ra: float

    # Declination, degrees.
    dec: float

    # Distance from the Earth, astronomical units.
    r: float

    def __init__(self, ra=0, dec=0, r=0) -> None:
        self.ra = ra
        self.dec = dec
        self.r = r

    def __repr__(self) -> str:
        return f"(ra: {self.ra}, dec: {self.dec}, r: {self.r})"

    def __sub__(self, other):
        return SunPosition(
            ra=angle_delta(self.ra, other.ra),
            dec=angle_delta(self.dec, other.dec),
            r=self.r - other.r,
        )


def get_sun_astropy(time: Time) -> list[SunPosition]:
    """
    Calculate sun positions for the given time points using Astropy
    """
    positions = []
    for sun in astropy.coordinates.get_sun(time=time):
        positions.append(
            SunPosition(
                ra=sun.ra.to(u.radian).value,
                dec=sun.dec.to(u.radian).value,
                r=sun.distance.to(u.AU).value,
            )
        )
    return positions


def tete_to_gcrs(tete: SunPosition, time: Time) -> SunPosition:
    """
    Convert sun position from TETE to GCRS
    """

    if False:
        # Conversion using the Atropy's frame conversion.
        # This is considered the ground-truth conversion here, which is then
        # gets broken down into smaller steps and optimized out without using
        # the Astropy utilities.
        tete_frame = astropy.coordinates.TETE(
            ra=tete.ra * u.rad,
            dec=tete.dec * u.rad,
            distance=tete.r * u.AU,
            obstime=time,
        )
        sun_gcrs = tete_frame.transform_to(
            astropy.coordinates.GCRS(obstime=time)
        )
        return SunPosition(
            ra=sun_gcrs.ra.to(u.radian).value,
            dec=sun_gcrs.dec.to(u.radian).value,
            r=sun_gcrs.distance.to(u.AU).value,
        )

    tete_cartesian = astropy.coordinates.spherical_to_cartesian(
        r=tete.r, lat=tete.dec, lon=tete.ra
    )

    # The code is based on the simplified tete_to_gcrs) from Astropy, omitting
    # the parts of TETE->GCRS conversion related on an observer location and
    # velocity.

    jd1, jd2 = time.tt.jd1, time.tt.jd2
    rbpn = erfa.pnm06a(jd1, jd2)

    gcrs_cartesian = tete_cartesian @ rbpn
    gcrs_spherical = astropy.coordinates.cartesian_to_spherical(*gcrs_cartesian)

    return SunPosition(
        ra=gcrs_spherical[2].value,
        dec=gcrs_spherical[1].value,
        r=gcrs_spherical[0].value,
    )


def approximate_sun_position(time: Time) -> SunPosition:
    """
    Calculate an approximate sun position at the given time point
    """

    JD = Time(time, scale="tt").jd

    # First, compute D, the number of days and fraction (+ or –) from the
    # epoch referred to as "J2000.0", which is 2000 January 1.5, Julian date
    # 2451545.0.
    D = JD - 2451545.0

    # Mean anomaly of the Sun.
    g_deg = 357.529 + 0.98560028 * D
    g_rad = degrees_to_radians(g_deg)

    # Mean longitude of the Sun.
    q_deg = 280.459 + 0.98564736 * D
    # q_rad = degrees_to_radians(q_deg)

    # Geocentric apparent ecliptic longitude of the Sun (adjusted for
    # aberration).
    L_deg = q_deg + 1.915 * math.sin(g_rad) + 0.020 * math.sin(2 * g_rad)
    L_rad = degrees_to_radians(L_deg)

    # The Sun's ecliptic latitude, b, can be approximated by b=0.

    # The distance of the Sun from the Earth, R, in astronomical units (AU).
    R = 1.00014 - 0.01671 * math.cos(g_rad) - 0.00014 * math.cos(2 * g_rad)

    # The mean obliquity of the ecliptic.
    e_deg = 23.439 - 0.00000036 * D
    e_rad = degrees_to_radians(e_deg)

    # Then the Sun's right ascension, RA.
    #
    # RA is always in the same quadrant as L. If the numerator and denominator
    # on the right side of the expression for RA are used in a double-argument
    # arctangent function (e.g., "atan2"), the proper quadrant will be obtained.
    RA_rad = math.atan2(math.cos(e_rad) * math.sin(L_rad), math.cos(L_rad))

    # The Sun's declination, d.
    sin_d = math.sin(e_rad) * math.sin(L_rad)
    d_rad = math.asin(sin_d)

    # The paper suggests the result is in equatorial coordinate system, which
    # the closest one seems to be Astropy's TETE. Convert it to GCRS to match
    # the result of the Astropy's get_sun().
    sun_tete = SunPosition(ra=RA_rad, dec=d_rad, r=R)
    sun_gcrs = tete_to_gcrs(sun_tete, time)

    return sun_gcrs


def get_sun_approximate(time: Time) -> list[SunPosition]:
    """
    Calculate approximate sun positions for the given time points
    """

    if True:
        pool = Pool(processes=8)
        result = pool.map(approximate_sun_position, time)
        pool.close()
        pool.join()
        return result

    positions = []
    for time_point in time:
        positions.append(approximate_sun_position(time_point))
    return positions


def run_tests() -> None:
    """
    Run tests to ensure self-consistency.

    If any of he tests failed the function aborts the program execution with
    a non-zero exit code.
    """
    result = unittest.main(exit=False).result
    if not result.wasSuccessful():
        sys.exit(1)

    start_time = Time("1800-01-01T00:00:00.000", scale="tt", format="isot")
    end_time = Time("2200-01-01T00:00:00.000", scale="tt", format="isot")
    time_delta = TimeDelta(0.1 * u.day)

    num_time_points = int((end_time - start_time + time_delta) / time_delta)
    times_array = np.linspace(start_time, end_time, num_time_points)
    times = Time(times_array)

    print("Calculating expected coordinates using Astropy...")
    sun_astropy = get_sun_astropy(times)
    print("Calculating approximate coordinates...")
    sun_approximate = get_sun_approximate(times)

    print("Preparing data for the plot...")
    time_labels = []
    error_RA = []
    error_dec = []
    error_r = []
    for time, expected, approximate in zip(times, sun_astropy, sun_approximate):
        time_labels.append(time.datetime)

        delta = approximate - expected
        error_RA.append(radians_to_arcsec(delta.ra))
        error_dec.append(radians_to_arcsec(delta.dec))
        error_r.append(delta.r)

    plt.title("Sun prediction error, arcsec")
    plt.plot(time_labels, error_RA, color="red", label="RA")
    plt.plot(time_labels, error_dec, color="green", label="dec")
    plt.legend()
    plt.show()


def main() -> None:
    run_tests()


if __name__ == "__main__":
    main()
