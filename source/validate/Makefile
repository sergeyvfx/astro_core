# Copyright (c) 2022 astro core authors
#
# SPDX-License-Identifier: MIT-0

# Example:
#
#   ERFA=/opt/local/erfa make transform_teme_to_itrf

BINDIR=$(CURDIR)/bin

transform_teme_to_itrf:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/transform_teme_to_itrf transform_teme_to_itrf.c -lerfa

transform_itrf_to_gcrf:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/transform_itrf_to_gcrf transform_itrf_to_gcrf.c -lerfa

celestial_intermediate_coordinate:
	mkdir -p $(BINDIR)
	clang -I${ERFA}/include -L${ERFA}/lib -o $(BINDIR)/celestial_intermediate_coordinate celestial_intermediate_coordinate.c -lerfa
