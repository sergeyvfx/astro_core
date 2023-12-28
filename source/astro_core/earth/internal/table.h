// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#pragma once

#include "astro_core/table/paged_table.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace earth_internal {

template <class RowType>
using Table = experimental::PagedTable<RowType, 32>;

}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
