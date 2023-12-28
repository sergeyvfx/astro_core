// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// JSON parsing functionality.
//
// A small header-only library, which is not intended to be exposed to a public
// API.

#pragma once

// Use the Niels Lohmann's JSON library.
//
// TODO(sergey): It might not be the most friendly for the low-resource embedded
// style of platforms this project is aiming to be friendly with.
//
// TODO(sergey): Look into avoiding possible name collision. Ideally would put
// the JSON implementation to a dedicated namespace, but the code in the JSON
// library makes it hard due to direct use of "::nlohmann".
#include "astro_core/base/internal/json/json.hpp"

namespace astro_core {

using json = nlohmann::json;

}  // namespace astro_core
