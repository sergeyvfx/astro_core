Astro Core
==========

Satellite tracking library for C++

This library aims to solve typical use-cases for satellite and other celestial
objects tracking for radio HAM. Additionally, it strives to have fully
documented algorithms implementations, with exact references to formulas from
whitepapers and books.

The library only provides core functionality, and some glue logic still needs to
be implemented in the application. For example, the application which uses this
library needs to take care of leap second and Earth orientation tables.

License
-------

The Astro Core library is licensed under the terms of the MIT license.
See [LICENSE](LICENSE) for more information.

Acknowledgments
---------------

- IERS, which provides Earth orientation tables, including description of the
  provided data tables: https://www.iers.org/

  Note: The Astro Core library mainly uses information table structure provided
  by IERS. There are various tables in the `data` folder, but they are only used
  for regression tests. For the proper operation in the user application it is
  to provide up-to-date IERS tables.

- SatNOGS, which provides machine-readable information about satellites and
  their transmitters: https://satnogs.org/

  Note: The Astro Core project only provides functionality to parse data
  provided by SatNOGS. The library itself contains no information about
  satellites.

- CelesTrak, and David Vallado's SGP4 model library:
  https://celestrak.org/publications/AIAA/2006-6753/

  The satellite prediction functionality in Astro Core is implemented using
  Vallado's SGP4 library.

- ERFA, which is used to verify result of the algorithms implemented in the
  Astro Core: https://github.com/liberfa/erf

- The Astropy Project, which is used to verify result of the algorithms
  implemented in the Astro Core: https://www.astropy.org/
