// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// A thin wrappers around actual coordinate or velocity representation which
// allows read or read-write access to the field, as well as conversion between
// different representations.
//
// The underlying value can be std::optional. Accessing representation will
// access the contained value of the optional.

#pragma once

#include <optional>
#include <type_traits>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class Cartesian;
class Spherical;

namespace representation_view_internal {

// Resolve contained value type.
// If T is std::optional gives its value_type, T otherwise.
template <class T, bool is_specialized>
struct ContainedValueType;

template <class T>
struct ContainedValueType<T, true> {
  using Type = T;
};

template <class T>
struct ContainedValueType<std::optional<T>, true> {
  using Type = T;
};

// Return contained value.
// Returns value.value() if T is std::optional, and value otherwise.
template <class T>
inline auto RemoveOptional(T& value) -> T& {
  return value;
}
template <class T>
inline auto RemoveOptional(const T& value) -> const T& {
  return value;
}
template <class T>
inline auto RemoveOptional(std::optional<T>& value) -> T& {
  return value.value();
}
template <class T>
inline auto RemoveOptional(const std::optional<T>& value) -> const T& {
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

}  // namespace representation_view_internal

// Read-only access to a coordinate or velocity representation which is stored
// in representation type T.
template <class T>
class ConstRepresentationView {
  using ValueType =
      typename representation_view_internal::ContainedValueType<T, true>::Type;

 public:
  explicit ConstRepresentationView(T& data) : data_(&data) {}

  inline auto has_value() const -> bool {
    return representation_view_internal::HasValue(*data_);
  }

  // Get value in cartesian representation.
  inline auto cartesian() const {
    return representation_view_internal::RemoveOptional(*data_).ToCartesian();
  }

  // Returns the contained value in cartesian representation if available,
  // another value otherwise
  inline auto cartesian_or(const ValueType& default_value) const {
    return has_value()
               ? cartesian()
               : representation_view_internal::RemoveOptional(default_value);
  }

  // Get value in spherical representation.
  inline auto spherical() const {
    return representation_view_internal::RemoveOptional(*data_).ToSpherical();
  }

  // Returns the contained value in spherical representation if available,
  // another value otherwise
  inline auto spherical_or(const ValueType& default_value) const {
    return has_value()
               ? spherical()
               : representation_view_internal::RemoveOptional(default_value);
  }

 protected:
  T* data_;
};

// Read-write access to a coordinate or velocity representation which is stored
// in representation type T.
template <class T>
class MutableRepresentationView : public ConstRepresentationView<T> {
  static_assert(!std::is_const_v<T*>);

 public:
  explicit MutableRepresentationView(T& data)
      : ConstRepresentationView<T>(data) {}

  // Convert value to the representation T and assign it to the underlying data.
  template <class Representation>
  inline auto operator=(const Representation& value) {
    *this->data_ = T(value);
    return *this;
  }
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
