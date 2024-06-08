// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/body/moon.h"

#include <array>
#include <span>

#include "astro_core/base/constants.h"
#include "astro_core/base/reverse_view.h"
#include "astro_core/coordinate/cartesian.h"
#include "astro_core/coordinate/spherical.h"
#include "astro_core/math/math.h"
#include "astro_core/numeric/numeric.h"
#include "astro_core/numeric/polynomial.h"
#include "astro_core/time/format/julian_date.h"
#include "astro_core/time/scale.h"
#include "astro_core/time/time.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

// Multipliers for the fundamental arguments D, M, M_prime, and F  for the
// series expansions defined by tables 47.A and 47.B.
//
// [Meeus1998] Page 339, Table 47.A.
// [Meeus1998] Page 341, Table 47.B.
struct TableMultipliers {
  int D;
  int M;
  int M_prime;
  int F;
};

// Row of a table for coefficients for series corrections of Σl and Σr.
// [Meeus1998] Page 339, Table 47.A.
struct TableSigmaLRRow {
  TableMultipliers mul;

  // Coefficients of the sine of the argument, degrees.
  double sin_sigma_l;

  // Coefficients of the cosine of the argument, meter.
  double cos_sigma_r;
};

// Table for coefficients for series corrections of Σl and Σr.
//
// [Meeus1998] Page 339, Table 47.A.
//
// NOTE: The coefficient for the Σl is preemptively divided by 1000000.
// clang-format off
constexpr auto kTableSigmaLR = std::to_array<TableSigmaLRRow>({
    {{0,  0,  1,  0},  6.288774, -20905355.0},
    {{2,  0, -1,  0},  1.274027,  -3699111.0},
    {{2,  0,  0,  0},  0.658314,  -2955968.0},
    {{0,  0,  2,  0},  0.213618,   -569925.0},
    {{0,  1,  0,  0}, -0.185116,     48888.0},
    {{0,  0,  0,  2}, -0.114332,     -3149.0},
    {{2,  0, -2,  0},  0.058793,    246158.0},
    {{2, -1, -1,  0},  0.057066,   -152138.0},
    {{2,  0,  1,  0},  0.053322,   -170733.0},
    {{2, -1,  0,  0},  0.045758,   -204586.0},
    {{0,  1, -1,  0}, -0.040923,   -129620.0},
    {{1,  0,  0,  0}, -0.034720,    108743.0},
    {{0,  1,  1,  0}, -0.030383,    104755.0},
    {{2,  0,  0, -2},  0.015327,     10321.0},
    {{0,  0,  1,  2}, -0.012528,         0.0},
    {{0,  0,  1, -2},  0.010980,     79661.0},
    {{4,  0, -1,  0},  0.010675,    -34782.0},
    {{0,  0,  3,  0},  0.010034,    -23210.0},
    {{4,  0, -2,  0},  0.008548,    -21636.0},
    {{2,  1, -1,  0}, -0.007888,     24208.0},
    {{2,  1,  0,  0}, -0.006766,     30824.0},
    {{1,  0, -1,  0}, -0.005163,     -8379.0},
    {{1,  1,  0,  0},  0.004987,    -16675.0},
    {{2, -1,  1,  0},  0.004036,    -12831.0},
    {{2,  0,  2,  0},  0.003994,    -10445.0},
    {{4,  0,  0,  0},  0.003861,    -11650.0},
    {{2,  0, -3,  0},  0.003665,     14403.0},
    {{0,  1, -2,  0}, -0.002689,     -7003.0},
    {{2,  0, -1,  2}, -0.002602,         0.0},
    {{2, -1, -2,  0},  0.002390,     10056.0},
    {{1,  0,  1,  0}, -0.002348,      6322.0},
    {{2, -2,  0,  0},  0.002236,     -9884.0},
    {{0,  1,  2,  0}, -0.002120,      5751.0},
    {{0,  2,  0,  0}, -0.002069,         0.0},
    {{2, -2, -1,  0},  0.002048,     -4950.0},
    {{2,  0,  1, -2}, -0.001773,      4130.0},
    {{2,  0,  0,  2}, -0.001595,         0.0},
    {{4, -1, -1,  0},  0.001215,     -3958.0},
    {{0,  0,  2,  2}, -0.001110,         0.0},
    {{3,  0, -1,  0}, -0.000892,      3258.0},
    {{2,  1,  1,  0}, -0.000810,      2616.0},
    {{4, -1, -2,  0},  0.000759,     -1897.0},
    {{0,  2, -1,  0}, -0.000713,     -2117.0},
    {{2,  2, -1,  0}, -0.000700,      2354.0},
    {{2,  1, -2,  0},  0.000691,         0.0},
    {{2, -1,  0, -2},  0.000596,         0.0},
    {{4,  0,  1,  0},  0.000549,     -1423.0},
    {{0,  0,  4,  0},  0.000537,     -1117.0},
    {{4, -1,  0,  0},  0.000520,     -1571.0},
    {{1,  0, -2,  0}, -0.000487,     -1739.0},
    {{2,  1,  0, -2}, -0.000399,         0.0},
    {{0,  0,  2, -2}, -0.000381,     -4421.0},
    {{1,  1,  1,  0},  0.000351,         0.0},
    {{3,  0, -2,  0}, -0.000340,         0.0},
    {{4,  0, -3,  0},  0.000330,         0.0},
    {{2, -1,  2,  0},  0.000327,         0.0},
    {{0,  2,  1,  0}, -0.000323,      1165.0},
    {{1,  1, -1,  0},  0.000299,         0.0},
    {{2,  0,  3,  0},  0.000294,         0.0},
    {{2,  0, -1, -2},  0.000000,      8752.0},
});
// clang-format on

// Row of a table for coefficients for series corrections of Σb.
// [Meeus1998] Page 341, Table 47.B.
struct TableSigmaBRow {
  TableMultipliers mul;

  // Coefficients of the sine of the argument, degrees.
  double sin_sigma_b;
};

// Table for coefficients for series corrections of Σb.
//
// [Meeus1998] Page 341, Table 47.B.
//
// NOTE: The coefficient for the Σb is preemptively divided by 1000000.
// clang-format off
constexpr auto kTableSigmaB = std::to_array<TableSigmaBRow>({
    {{0,  0,  0,  1},  5.128122},
    {{0,  0,  1,  1},  0.280602},
    {{0,  0,  1, -1},  0.277693},
    {{2,  0,  0, -1},  0.173237},
    {{2,  0, -1,  1},  0.055413},
    {{2,  0, -1, -1},  0.046271},
    {{2,  0,  0,  1},  0.032573},
    {{0,  0,  2,  1},  0.017198},
    {{2,  0,  1, -1},  0.009266},
    {{0,  0,  2, -1},  0.008822},
    {{2, -1,  0, -1},  0.008216},
    {{2,  0, -2, -1},  0.004324},
    {{2,  0,  1,  1},  0.004200},
    {{2,  1,  0, -1}, -0.003359},
    {{2, -1, -1,  1},  0.002463},
    {{2, -1,  0,  1},  0.002211},
    {{2, -1, -1, -1},  0.002065},
    {{0,  1, -1, -1}, -0.001870},
    {{4,  0, -1, -1},  0.001828},
    {{0,  1,  0,  1}, -0.001794},
    {{0,  0,  0,  3}, -0.001749},
    {{0,  1, -1,  1}, -0.001565},
    {{1,  0,  0,  1}, -0.001491},
    {{0,  1,  1,  1}, -0.001475},
    {{0,  1,  1, -1}, -0.001410},
    {{0,  1,  0, -1}, -0.001344},
    {{1,  0,  0, -1}, -0.001335},
    {{0,  0,  3,  1},  0.001107},
    {{4,  0,  0, -1},  0.001021},
    {{4,  0, -1,  1},  0.000833},
    {{0,  0,  1, -3},  0.000777},
    {{4,  0, -2,  1},  0.000671},
    {{2,  0,  0, -3},  0.000607},
    {{2,  0,  2, -1},  0.000596},
    {{2, -1,  1, -1},  0.000491},
    {{2,  0, -2,  1}, -0.000451},
    {{0,  0,  3, -1},  0.000439},
    {{2,  0,  2,  1},  0.000422},
    {{2,  0, -3, -1},  0.000421},
    {{2,  1, -1,  1}, -0.000366},
    {{2,  1,  0,  1}, -0.000351},
    {{4,  0,  0,  1},  0.000331},
    {{2, -1,  1,  1},  0.000315},
    {{2, -2,  0, -1},  0.000302},
    {{0,  0,  1,  3}, -0.000283},
    {{2,  1,  1, -1}, -0.000229},
    {{1,  1,  0, -1},  0.000223},
    {{1,  1,  0,  1},  0.000223},
    {{0,  1, -2, -1}, -0.000220},
    {{2,  1, -1, -1}, -0.000220},
    {{1,  0,  1,  1}, -0.000185},
    {{2, -1, -2, -1},  0.000181},
    {{0,  1,  2,  1}, -0.000177},
    {{4,  0, -2, -1},  0.000176},
    {{4, -1, -1, -1},  0.000166},
    {{1,  0,  1, -1}, -0.000164},
    {{4,  0,  1, -1},  0.000132},
    {{1,  0, -1, -1}, -0.000119},
    {{4, -1,  0, -1},  0.000115},
    {{2, -2,  0,  1},  0.000107},
});
// clang-format on

struct FundamentalArguments {
  double D;        // Mean elongation of the Moon,
  double M;        // Sun's mean anomaly.
  double M_prime;  // Moon's mean anomaly.
  double F;        // Moon's argument of latitude.
};

// Calculate linear combination of the fundamental coefficients and the weights
// from the correction table.
inline auto CalculateSeriesArgument(
    const TableMultipliers& mul,
    const FundamentalArguments& fundamental_coefficients) -> double {
  return mul.D * fundamental_coefficients.D +
         mul.M * fundamental_coefficients.M +
         mul.M_prime * fundamental_coefficients.M_prime +
         mul.F * fundamental_coefficients.F;
}

// Calculate correction factor caused by eccentricity of the Earth orbit used
// for the series expansions.
inline auto CalculateSeriesEccentricityCorrection(const TableMultipliers& mul,
                                                  const double E,
                                                  const double E2) -> double {
  if (mul.M == 1 || mul.M == -1) {
    return E;
  } else if (mul.M == 2 || mul.M == -2) {
    return E2;
  }
  return 1.0;
}

// The implementation follows the algorithm described in [Meeus1998] Page 337.
auto GetMeeusMoonCoordinate(const JulianDate& jd_tt) -> MeeusMoonCoordinate {
  // [Meeus1998] Page 143, Eq. (22.1).
  const double T = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  // Moon's mean longitude, referred to the mean equinox of the date, and
  // including the term of the effect of the light-time (-0".70).
  // [Meeus1998] Page 338, Eq. (47.1).
  //
  // The 0-degree coefficient is from [Simon1994] Page 669, Eq. (a.2), the rest
  // of the coefficients are from [Meeus1998] Page 338, Eq. (47.1). This follows
  // the ERFA implementation of moon98 revision 2021 May 11.
  //
  // TODO(sergey): Figure out why there is such parameterization used in ERFA.
  const double L_prime =
      DegreesToRadians(ReduceDegrees(Polynomial(T,
                                                218.31665436,
                                                481267.88123421,
                                                -0.0015786,
                                                1.0 / 538841.0,
                                                -1.0 / 65194000.0)));

  // Mean elongation of the Moon,
  // [Meeus1998] Page 338, Eq. (47.2).
  const double D =
      DegreesToRadians(ReduceDegrees(Polynomial(T,
                                                297.8501921,
                                                445267.1114034,
                                                -0.0018819,
                                                1.0 / 545868.0,
                                                -1.0 / 113065000.0)));

  // Sun's mean anomaly.
  // [Meeus1998] Page 338, Eq. (47.3).
  const double M = DegreesToRadians(ReduceDegrees(
      Polynomial(T, 357.5291092, 35999.0502909, -0.0001536, 1.0 / 24490000.0)));

  // Moon's mean anomaly.
  // [Meeus1998] Page 338, Eq. (47.4).
  const double M_prime =
      DegreesToRadians(ReduceDegrees(Polynomial(T,
                                                134.9633964,
                                                477198.8675055,
                                                0.0087414,
                                                1.0 / 69699.0,
                                                -1.0 / 14712000.0)));

  // Moon's argument of latitude (mean distance of the Moon from its ascending
  // node).
  // [Meeus1998] Page 338, Eq. (47.5).
  const double F =
      DegreesToRadians(ReduceDegrees(Polynomial(T,
                                                93.2720950,
                                                483202.0175233,
                                                -0.0036539,
                                                1.0 / 3526000.0,
                                                1.0 / 863310000.0)));

  // Three further arguments.
  const double A1 = DegreesToRadians(119.75 + 131.849 * T);
  const double A2 = DegreesToRadians(53.09 + 479264.290 * T);
  const double A3 = DegreesToRadians(313.45 + 481266.484 * T);

  // Correction factor E for the  eccentricity of the Earth's orbit around the
  // Sun.
  // [Meeus1998] Page 338, Eq. (47.6).
  const double E = Polynomial(T, 1.0, -0.002516, -0.0000074);
  const double E2 = E * E;

  // TODO(sergey): Calculate derivatives.

  // Additive terms to Σl and to Σb.
  // The terms involving A_1, are due to the action of Venus, The term involving
  // A2, is due to Jupiter, while those involving L' are due ot the flattening
  // of the Earth.
  //
  // [Meeus1998] Page 342.
  //
  // NOTE: The coefficients are preemptively divided by 1000000 to avoid big
  // numbers during intermediate calculations. This follows the implementation
  // in ERFA.
  const double add_sigma_l =
      0.003958 * Sin(A1) + 0.001962 * Sin(L_prime - F) + 0.000318 * Sin(A2);
  const double add_sigma_b = -0.002235 * Sin(L_prime) + 0.000382 * Sin(A3) +
                             0.000175 * Sin(A1 - F) + 0.000175 * Sin(A1 + F) +
                             0.000127 * Sin(L_prime - M_prime) -
                             0.000115 * Sin(L_prime + M_prime);

  // Series expansions.
  //
  // Calculate the sums Σl and Σr of the terms given in Table 47.A, and the sum
  // Σb of the terms given in Table 47.B. The argument of each sine (for Σl and
  // Σb) and cosine (for Σr) is a linear combination of the four fundamental
  // arguments D, M, M', and F.
  //
  // The terms whose argument contains the angle M depend on the eccentricity of
  // the Earth's orbit around the Sun, which presently is decreasing with time.
  // To take this effect into account, multiply the terms whose argument
  // contains M or M' by E, and those containing 2*M or 2*M' by E².
  //
  // [Meeus1998] Page 338.

  const FundamentalArguments fundamental_coefficients = {
      .D = D, .M = M, .M_prime = M_prime, .F = F};

  // Series correction for Σl and Σr.
  double sigma_l = add_sigma_l;
  double sigma_r = 0.0;
  for (const TableSigmaLRRow& row : reverse_view(kTableSigmaLR)) {
    const double argument = row.mul.D * D + row.mul.M * M +
                            row.mul.M_prime * M_prime + row.mul.F * F;

    const double eccentricity_correction =
        CalculateSeriesEccentricityCorrection(row.mul, E, E2);

    sigma_l += row.sin_sigma_l * eccentricity_correction * Sin(argument);
    sigma_r += row.cos_sigma_r * eccentricity_correction * Cos(argument);
  }

  // Series correction for Σb.
  double sigma_b = add_sigma_b;
  for (const TableSigmaBRow& row : reverse_view(kTableSigmaB)) {
    const double argument =
        CalculateSeriesArgument(row.mul, fundamental_coefficients);

    const double eccentricity_correction =
        CalculateSeriesEccentricityCorrection(row.mul, E, E2);

    sigma_b += row.sin_sigma_b * eccentricity_correction * Sin(argument);
  }

  // The coordinates of the Moon are then given by
  // [Meeus1998] Page 342.
  const double lambda = L_prime + DegreesToRadians(sigma_l);  // Radians.
  const double beta = DegreesToRadians(sigma_b);              // Radians.
  const double delta = 385000560.0 + sigma_r;                 // Meter.

  return MeeusMoonCoordinate{.lambda = lambda, .beta = beta, .delta = delta};
}

// Polynomial calculation for Fukushima-Williams precession angles.
// Reduces the arcseconds and converts them to radians.
template <class... Args>
auto LunisolarNutationPolynomial(const double t, Args... args) -> double {
  return ArcsecToRadians(ReduceArcsec(Polynomial(t, args...)));
}

}  // namespace

// The implementation follows the algorithm described in [Meeus1998] Page 337.
auto GetMeeusMoonCoordinate(const Time& time) -> MeeusMoonCoordinate {
  const Time time_tt = time.ToScale(TimeScale::kTT);
  const JulianDate jd_tt = time_tt.AsFormat<JulianDate>();

  return GetMeeusMoonCoordinate(jd_tt);
}

auto GetMoonCoordinate(const Time& time) -> GCRF {
  const Time time_tt = time.ToScale(TimeScale::kTT);
  const JulianDate jd_tt = time_tt.AsFormat<JulianDate>();

  const MeeusMoonCoordinate meeus = GetMeeusMoonCoordinate(jd_tt);

  // TODO(sergey): There is probably a way to transform angles in spherical
  // coordinates without conversion to cartesian representation.
  const Cartesian r = Spherical({.latitude = meeus.beta,
                                 .longitude = meeus.lambda,
                                 .distance = meeus.delta})
                          .ToCartesian();

  // [Meeus1998] Page 143, Eq. (22.1).
  const double T = double((jd_tt - constants::kJulianDateEpochJ2000) /
                          constants::kNumDaysInJulianCentury);

  // Fukushima-Williams precession angles.
  // [Vallado2013] Page 218, Eq. (3-74).
  //
  // TODO(sergey): Use CalculatePrecessionAngles06()?
  const double gamma = LunisolarNutationPolynomial(
      T, -0.052928, 10.556378, 0.4932044, -0.00031238, -2.788e-6, 2.60e-8);
  const double phi = LunisolarNutationPolynomial(
      T, 84381.412819, -46.811016, 0.0511268, 0.00053289, -4.40e-7, -1.76e-8);
  const double psi = LunisolarNutationPolynomial(
      T, -0.041775, 5038.481484, 1.5584175, -0.00018522, -2.6452e-5, -1.48e-8);

  // Rotation matrix from Mean ecliptic coordinates to GCRS.
  // [Vallado2013] Page 218, Eq. (3-74).
  // [Hilton2006] Page 360, Eq. (11).
  //
  // TODO(sergey): Use PrecessionRotation()?
  const Mat3 mean_ecliptic_to_gcrs = ROT3(-gamma) * ROT1(-phi) * ROT3(psi);

  const Vec3 r_gcrf = mean_ecliptic_to_gcrs * r;

  // TODO(sergey): Convert speed.

  return GCRF({.observation_time = time, .position = r_gcrf});
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
