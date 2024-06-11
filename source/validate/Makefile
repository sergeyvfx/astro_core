# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

# Example:
#
#   ERFA=/opt/local/erfa make transform_teme_to_itrf

BINDIR=$(CURDIR)/bin

# Representation.

representation_cartesian_to_spherical:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/representation_cartesian_to_spherical representation/cartesian_to_spherical.c -lerfa -lm

representation_spherical_to_cartesian:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/representation_spherical_to_cartesian representation/spherical_to_cartesian.c -lerfa -lm

# Frame conversion.

transform_teme_to_itrf:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/transform_teme_to_itrf transform_teme_to_itrf.c -lerfa -lm

transform_itrf_to_gcrf:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/transform_itrf_to_gcrf transform_itrf_to_gcrf.c -lerfa -lm

celestial_intermediate_coordinate:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/celestial_intermediate_coordinate celestial_intermediate_coordinate.c -lerfa -lm

# Solar system bodies.

body_moon:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/body_moon body/moon.c -lerfa -lm
