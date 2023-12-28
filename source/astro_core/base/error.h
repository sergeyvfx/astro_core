// Copyright (c) 2021 astro core authors
//
// SPDX-License-Identifier: MIT

// Commonly used error codes.

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

enum class Error {
  // Indication of a successful operation result.
  kOk,

  // Some resource has exhausted. For example, the file system went out of space
  // when saving file, or there is no enough space in a buffer provided to save
  // some data.
  kResourceExhausted,

  // Argument value is valid.
  kInvalidValue,
};

}
}  // namespace astro_core
