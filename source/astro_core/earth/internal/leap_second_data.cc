// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/earth/leap_second_data.h"

#include <algorithm>
#include <array>

#include "astro_core/base/constants.h"
#include "astro_core/earth/leap_second_table.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Calculation of leap second using the historical table.
//
// Relevant discussion:
//   Update UTC-TAI for years between 1961 and 1972
//   https://github.com/skyfielders/python-skyfield/issues/679
//
// IERS listing of historic offsets between TAI and UTC
// https://hpiers.obspm.fr/eop-pc/earthor/utc/TAI-UTC_tab.html
//
//
// Limits of validity (at 0h UTC) TAI-UTC (MJD = Modified Julian Day)
// ____________________________________________________________________________
//
// 1961  Jan.  1 - 1961  Aug.  1    1.422 818 0s + (MJD - 37 300) x 0.001 296s
//       Aug.  1 - 1962  Jan.  1    1.372 818 0s +        ""
// 1962  Jan.  1 - 1963  Nov.  1    1.845 858 0s + (MJD - 37 665) x 0.001 123 2s
// 1963  Nov.  1 - 1964  Jan.  1    1.945 858 0s +        ""
// 1964  Jan.  1 -       April 1    3.240 130 0s + (MJD - 38 761) x 0.001 296s
//       April 1 -       Sept. 1    3.340 130 0s +        ""
//       Sept. 1 - 1965  Jan.  1    3.440 130 0s +        ""
// 1965  Jan.  1 -       March 1    3.540 130 0s +        ""
//       March 1 -       Jul.  1    3.640 130 0s +        ""
//       Jul.  1 -       Sept. 1    3.740 130 0s +        ""
//       Sept. 1 - 1966  Jan.  1    3.840 130 0s +        ""
// 1966  Jan.  1 - 1968  Feb.  1    4.313 170 0s + (MJD - 39 126) x 0.002 592s
// 1968  Feb.  1 - 1972  Jan.  1    4.213 170 0s +        ""

// Representation of row of a table from above.
// It is re-organized for a more convenient use from a program.
struct HistoricalRow {
  // MJD at which the correction starts to take an effect, measured in UTC time
  // scale. The correction is valid until the MJD at the next row.
  double mjd_utc;

  // Equivalent of the mjd_utc in the TAI time scale.
  DoubleDouble mjd_tai;

  // Constant offset, in seconds.
  double offset;

  // Delta which is subtracted from the MJD.
  double mjd_delta;

  // Multiplier which denotes the rate of the leap second accumulation.
  double rate;
};

constexpr std::array kHistoricalTable = std::to_array<HistoricalRow>({
    {37300.0, {37300.0, 0.000016467798559461}, 1.422818, 37300, 0.0012960},
    {37512.0, {37512.0, 0.000019069098925684}, 1.372818, 37300, 0.0012960},
    {37665.0, {37665.0, 0.000021364096028265}, 1.845858, 37665, 0.0011232},
    {38334.0, {38334.0, 0.000031218507501762}, 1.945858, 37665, 0.0011232},
    {38395.0, {38395.0, 0.000032011506846175}, 3.240130, 38761, 0.0012960},
    {38486.0, {38486.0, 0.000034533914003987}, 3.340130, 38761, 0.0012960},
    {38639.0, {38639.0, 0.000037986319512129}, 3.440130, 38761, 0.0012960},
    {38761.0, {38761.0, 0.000040973725845106}, 3.540130, 38761, 0.0012960},
    {38820.0, {38820.0, 0.000043016130803153}, 3.640130, 38761, 0.0012960},
    {38942.0, {38942.0, 0.000046003544412088}, 3.740130, 38761, 0.0012960},
    {39004.0, {39004.0, 0.000048090951167978}, 3.840130, 38761, 0.0012960},
    {39126.0, {39126.0, 0.000049920949095394}, 4.313170, 39126, 0.0025920},
    {39887.0, {39887.0, 0.000071593538450543}, 4.213170, 39126, 0.0025920},
    {41317.0, {41317.0, 0.000114493544970173}, 10.0, 0, 0},
});

// Check whether the given time point falls into the range covered by the
// historical leap second correction table.
auto IsHistoricalUTCTime(const ModifiedJulianDate& mjd_utc) -> bool {
  constexpr double kTableStartTime = kHistoricalTable.front().mjd_utc;
  constexpr double kTableEndTime = kHistoricalTable.back().mjd_utc;

  return mjd_utc >= kTableStartTime && mjd_utc < kTableEndTime;
}
auto IsHistoricalTAITime(const ModifiedJulianDate& mjd_tai) -> bool {
  constexpr DoubleDouble kTableStartTime = kHistoricalTable.front().mjd_tai;
  constexpr DoubleDouble kTableEndTime = kHistoricalTable.back().mjd_tai;

  return mjd_tai >= kTableStartTime && mjd_tai < kTableEndTime;
}

auto CalculateHistoricalTAIMinusUTCSecondsInUTCScale(
    const ModifiedJulianDate& mjd_utc) -> double {
  auto upper_it = std::lower_bound(
      kHistoricalTable.begin(),
      kHistoricalTable.end(),
      mjd_utc,
      [](const HistoricalRow& row, const ModifiedJulianDate& bound_mjd_utc) {
        return row.mjd_utc < bound_mjd_utc;
      });

  if (upper_it->mjd_utc > mjd_utc) {
    --upper_it;
  }

  const HistoricalRow& row = *upper_it;

  return row.offset + double(mjd_utc - row.mjd_delta) * row.rate;
}

auto CalculateHistoricalTAIMinusUTCSecondsInTAIScale(
    const ModifiedJulianDate& mjd_tai) -> double {
  // clang-format off
  //
  // tai_minus_utc = offset + (mjd_utc - delta) * rate
  //
  // mjd_tai = mjd_utc + tai_minus_utc/86400
  // mjd_utc = mjd_tai - tai_minus_utc/86400
  //
  // tai_minus_utc = offset + (mjd_tai - tai_minus_utc/86400 - delta) * rate
  // tai_minus_utc = offset + (mjd_tai -  delta) * rate - tai_minus_utc/86400 * rate
  // tai_minus_utc + tai_minus_utc/86400 * rate = offset + (mjd_tai -  delta) * rate
  // tai_minus_utc * (1 + rate/86400) = offset + (mjd_tai -  delta) * rate
  // tai_minus_utc = (offset + (mjd_tai -  delta) * rate) / (1.0 + rate/86400)
  //
  // clang-format on

  auto upper_it = std::lower_bound(
      kHistoricalTable.begin(),
      kHistoricalTable.end(),
      mjd_tai,
      [](const HistoricalRow& row, const ModifiedJulianDate& bound_mjd_tai) {
        return row.mjd_tai < bound_mjd_tai;
      });

  if (upper_it->mjd_tai > mjd_tai) {
    --upper_it;
  }

  const HistoricalRow& row = *upper_it;

  return (row.offset + double(mjd_tai - row.mjd_delta) * row.rate) /
         (1.0 + row.rate / constants::kNumSecondsInDay);
}

}  // namespace

// Hide the implementation, so that it is possible to only have forward
// declaration of Table in the header.
LeapSecondData::LeapSecondData() = default;
LeapSecondData::~LeapSecondData() = default;

void LeapSecondData::SetTable(Table&& table) {
  shared_table_.Set(std::move(table));
}

auto LeapSecondData::LookupTAIMinusUTCSecondsInUTCScale(
    const ModifiedJulianDate& mjd_utc) const -> double {
  if (IsHistoricalUTCTime(mjd_utc)) {
    return CalculateHistoricalTAIMinusUTCSecondsInUTCScale(mjd_utc);
  }

  const SharedTable::LocalTable local_table = shared_table_.Load();
  if (!local_table) {
    return 0;
  }

  return local_table->LookupTAIMinusUTCSecondsInUTCScale(mjd_utc);
}

auto LeapSecondData::LookupTAIMinusUTCSecondsInTAIScale(
    const ModifiedJulianDate& mjd_tai) const -> double {
  if (IsHistoricalTAITime(mjd_tai)) {
    return CalculateHistoricalTAIMinusUTCSecondsInTAIScale(mjd_tai);
  }

  const SharedTable::LocalTable local_table = shared_table_.Load();
  if (!local_table) {
    return 0;
  }

  return local_table->LookupTAIMinusUTCSecondsInTAIScale(mjd_tai);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
