// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/time/time.h"

#include "astro_core/time/internal/scale_convert.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

auto Time::ToScale(const TimeScale scale) const -> Time {
  if (scale == scale_) {
    return *this;
  }

  return time_internal::ConvertToScale(*this, scale);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
