#!/usr/bin/env python3

# Copyright (c) 2023 astro core authors
#
# SPDX-License-Identifier: MIT-0

from math import sin, cos, radians

from astropy.time import Time
from astropy.coordinates import (
    ITRS,
    TEME,
    CartesianRepresentation,
    CartesianDifferential,
)
from astropy import units as u
from astropy.utils import iers

iers.conf.auto_download = False

t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")

r_teme = CartesianRepresentation(
    [4357.092619856639, 4500.439126822302, -2645.108425391841] * u.km
)
v_teme = CartesianDifferential(
    [-2.1768117558889037, 5.163121595591936, 5.215977759982141] * u.km / u.s
)

teme = TEME(r_teme.with_differentials(v_teme), obstime=t)
itrs = teme.transform_to(ITRS(obstime=t))

print(f"r_itrs: {itrs.cartesian}")
print(f"v_itrs: {itrs.velocity}")
