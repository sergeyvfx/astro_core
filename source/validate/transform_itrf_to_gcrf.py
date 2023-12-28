#!/usr/bin/env python3

# Copyright (c) 2023 astro core authors
#
# SPDX-License-Identifier: MIT-0

from math import sin, cos, radians

from astropy.time import Time
from astropy.coordinates import (
    ITRS,
    GCRS,
    CartesianRepresentation,
    CartesianDifferential,
)
from astropy import units as u
from astropy.utils import iers

iers.conf.auto_download = False

t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")

r_itrf = CartesianRepresentation(
    [
        -2801.428206798944302136,
        5602.703300938050233526,
        -2645.094088710325195279,
    ]
    * u.km
)
v_itrf = CartesianDifferential(
    [-5.184234346857372167, -0.137714270932494498, 5.215984905464025267]
    * u.km
    / u.s
)

itrs = ITRS(r_itrf.with_differentials(v_itrf), obstime=t)
gcrs = itrs.transform_to(GCRS(obstime=t))

print(f"r_gcrf: {gcrs.cartesian}")
print(f"v_gcrf: {gcrs.velocity}")

print("Manual conversion of spherical->cartesian:")
ra = radians(gcrs.ra.value)
dec = radians(gcrs.dec.value)
distance = gcrs.distance.value

x = cos(ra) * cos(dec) * distance
y = sin(ra) * cos(dec) * distance
z = sin(dec) * distance

print(x, y, z)
