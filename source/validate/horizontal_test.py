#!/usr/bin/env python3

# Copyright (c) 2023 astro core authors
#
# SPDX-License-Identifier: MIT-0

from astropy.coordinates import (
    EarthLocation,
    ITRS,
    CartesianDifferential,
    CartesianRepresentation,
    AltAz,
)
from astropy import units as u
from astropy.time import Time
import numpy as np

np.set_printoptions(precision=18, suppress=True)
t = Time("2022-08-10T22:00:00.000", scale="utc", format="isot")

# Position of GOES 16.
itrs_p_data = (
    10366753.696330964565277100,
    -40872357.989425994455814362,
    -7177.146307127579348162,
)
itrs_v_data = (
    -0.000123270733281970,
    -0.000265212595462799,
    0.000614554703409340,
)

# Construct ITRS frame of the GOES 16 satellite.
itrs_p = CartesianRepresentation(np.array(itrs_p_data) * u.m)
itrs_v = CartesianDifferential(np.array(itrs_v_data) * u.m / u.s)
itrs = ITRS(itrs_p.with_differentials(itrs_v), obstime=t)

# Earth locations of sites to calculate Azimuth/elevation from.
all_sites = [
    # Siding Spring observatory.
    EarthLocation.of_site("aao"),
    # Somewhere in Europe.
    EarthLocation(lat=50 * u.deg, lon=8 * u.deg),
]

for site in all_sites:
    site_itrs = site.itrs
    site_itrs_p = site_itrs.earth_location
    print(
        "Site : ("
        f"{site_itrs_p.x.value:0.18f}, "
        f"{site_itrs_p.y.value:0.18f}, "
        f"{site_itrs_p.z.value:0.18f})"
    )

    aa = itrs.transform_to(AltAz(obstime=t, location=site_itrs_p))

    print(f"Elevation: {aa.alt}")
    print(f"Azimuth: {aa.az}")
    print(f"Distance: {aa.distance}")
    print()
