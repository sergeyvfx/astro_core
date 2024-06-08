// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// A storage of coordinate or velocity is specific representation with a
// flexible access to this value in desired representation.
//
// Example:
//
//   Representation<Cartesian> position;
//   std::cout << "Cartesian: " << position.cartesian() << std::endl;
//   std::cout << "Spherical: " << position.spherical() << std::endl;
//
// The underlying value can be std::optional.

#pragma once

#include <optional>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Cartesian;
class Spherical;

namespace representation_internal {

// Resolve contained value type.
// If T is std::optional gives its value_type, T otherwise.
template <class T, bool is_specialized>
struct ContainedValueTypeHelper;

template <class T>
struct ContainedValueTypeHelper<T, true> {
  using Type = T;
};

template <class T>
struct ContainedValueTypeHelper<std::optional<T>, true> {
  using Type = T;
};

template <class T>
using ContainedValueType = typename ContainedValueTypeHelper<T, true>::Type;

// Return contained value.
//
// Returns value.value() if T is std::optional, and value otherwise.
//
// Exceptions
// std::bad_optional_access if the T is std::optional without contained value.
template <class T>
inline auto GetContainedValue(T& value) -> T& {
  return value;
}
template <class T>
inline auto GetContainedValue(const T& value) -> const T& {
  return value;
}
template <class T>
inline auto GetContainedValue(std::optional<T>& value) -> T& {
  return value.value();
}
template <class T>
inline auto GetContainedValue(const std::optional<T>& value) -> const T& {
  return value.value();
}

// Return true if the value variable actually contains variable.
// For the non-optional type it is always true. For the std::optional the call
// is redirected to std::optional::has_value().
template <class T>
inline auto HasValue(const T& /*value*/) -> bool {
  return true;
}
template <class T>
inline auto HasValue(const std::optional<T>& value) -> bool {
  return value.has_value();
}

}  // namespace representation_internal

template <class ValueType>
class Representation {
 public:
  Representation() = default;

  Representation(const ValueType& value) : value_(value) {}

  // Returns true if the representation has a value.
  //
  // If ValueType is std::optional<> then it is equivalent of returning the
  // contained value's has_value().
  // Otherwise it always returns true.
  inline auto HasValue() const -> bool {
    return representation_internal::HasValue(value_);
  }

  // Get value in cartesian representation.
  //
  // Exceptions
  // std::bad_optional_access if the ValueType is std::optional and the
  // representation contains no actual value.
  inline auto GetCartesian() const {
    return representation_internal::GetContainedValue(value_).ToCartesian();
  }

  // Returns the contained value in cartesian representation if available,
  // a provided default value.
  inline auto GetCartesianOr(
      const representation_internal::ContainedValueType<ValueType>&
          default_value) const {
    return HasValue() ? GetCartesian() : default_value;
  }

  // Get value in spherical representation.
  //
  // Exceptions
  // std::bad_optional_access if the ValueType is std::optional and the
  // representation contains no actual value.
  inline auto GetSpherical() const {
    return representation_internal::GetContainedValue(value_).ToSpherical();
  }

  // Returns the contained value in spherical representation if available,
  // a provided default value.
  inline auto GetSphericalOr(
      const representation_internal::ContainedValueType<ValueType>&
          default_value) const {
    return HasValue() ? GetSpherical() : default_value;
  }

  // Convert value to the representation ValueType and assign it to the
  // underlying data.
  template <class OtherValue>
  inline auto operator=(const OtherValue& value) {
    value_ = ValueType(value);
    return *this;
  }

 private:
  ValueType value_{};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
