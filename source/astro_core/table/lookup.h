// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Lookup utilities for data in tables.

#pragma once

#include <algorithm>
#include <cassert>
#include <span>
#include <type_traits>
#include <utility>

#include "astro_core/math/math.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Lookup row for the given key. The row with the highest key which is not
// exceeding the requested key is used to obtain the value.
//
// Clamping extrapolation strategy is used. This means that for the keys
// outside of what the table provides a value from the closest row is returned.
// For example, for a key prior to the first table row the first row is
// returned. Likewise, for a key past the last row of the table the last row is
// returned.
//
// If the table is empty then the behavior is undefined and could cause bad
// memory access.
template <auto KeyField, class Table, class KeyType>
auto LookupFloorRow(const Table& table, const KeyType& key) -> const
    typename Table::value_type& {
  using Row = typename Table::value_type;

  assert(!table.empty());

  const auto upper_it =
      std::lower_bound(table.begin(),
                       table.end(),
                       key,
                       [](const Row& row, const KeyType& bound_key) {
                         return row.*KeyField < bound_key;
                       });

  if (upper_it == table.end()) {
    // The key is past the last row of the table: use the last known value from
    // the table.
    return table.back();
  }

  // TODO(sergey): Investigate floating point precision of real life keys
  // passed here. Might need to perform some fuzzy with sub-nanosecond
  // tolerance.
  if ((*upper_it).*KeyField == key) {
    // The key is at the exact table row, use the value from the row.
    return *upper_it;
  }

  if (upper_it == table.begin()) {
    // The key is prior to the first row of the table. Clamp to the first row.
    return table.front();
  }

  // Use the previous row of the table (as it is the one with the highest key
  // which does not exceed the given one).
  return *(upper_it - 1);
}

// Lookup value for the given key. The row with the highest key which is not
// exceeding the requested key is used to obtain the value.
//
// Clamping extrapolation strategy is used. This means that for the keys
// outside of what the table provides a value from the closest row is returned.
// For example, for a date prior to the first table row the value from the first
// row is returned. Likewise, for a date past the last row of the table the
// value from the last row is returned.
//
// If the table is empty then the behavior is undefined and could cause bad
// memory access.
template <auto KeyField, auto ValueField, class Table, class KeyType>
auto LookupFloor(const Table& table, const KeyType& key) {
  const auto& row = LookupFloorRow<KeyField>(table, key);
  return row.*ValueField;
}

// Lookup value for the given key, performing linear interpolation.
//
// For the key which is in-between of two table rows the output value is the
// linear interpolation of values f the adjacent rows.
//
// When interpolation weight is calculated for the key, the key is converted to
// the KeyInterpolationType type. This type needs to support arithmetic
// operators like addition, subtraction, division, and multiplication.
//
// The value of the adjacent rows is used directly by the Lerp() functions.
// In practice this means that linear linear interpolation lookup of integral
// types is not likely to give the expected result.
//
// If the table is empty then the behavior is undefined and could cause bad
// memory access.
template <auto KeyField,
          auto ValueField,
          class KeyInterpolationType,
          class Table,
          class KeyType>
auto LinearInterpolate(const Table& table, const KeyType& key) {
  using Row = typename Table::value_type;
  using ValueType = std::decay_t<decltype(std::declval<Row>().*ValueField)>;

  assert(!table.empty());

  const auto upper_it =
      std::lower_bound(table.begin(),
                       table.end(),
                       key,
                       [](const Row& row, const KeyType& bound_key) {
                         return row.*KeyField < bound_key;
                       });

  if (upper_it == table.end()) {
    // The key is past the last row of the table: use the last known value from
    // the table.
    return table.back().*ValueField;
  }

  if (upper_it == table.begin()) {
    // The key is prior to the first row of the table. Clamp to the first row.
    return table.front().*ValueField;
  }

  const Row& row_a = *(upper_it - 1);
  const Row& row_b = *upper_it;

  const auto scalar_key = KeyInterpolationType(key);
  const auto scalar_key_a = KeyInterpolationType(row_a.*KeyField);
  const auto scalar_key_b = KeyInterpolationType(row_b.*KeyField);

  const auto weight =
      (scalar_key - scalar_key_a) / (scalar_key_b - scalar_key_a);

  return ValueType(Lerp(row_a.*ValueField, row_b.*ValueField, weight));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
