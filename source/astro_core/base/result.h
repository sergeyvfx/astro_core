// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// An optional contained value with an error information associated with it.
//
// The error is a required part of the Result in the case the value is not
// known. This error will provide details about reasoning why the result could
// not be calculated.
//
// It is possible to have result which has both value and error associated with
// it. In this case the result is considered to be ill-calculated, but it is
// allowed to access the partially calculated value.
//
// The result is considered to be successfully calculated if and only if there
// is no error associated with it.
//
// A common use case for the Result is the return value of a function which
// might fail and wants to communicate details about the failure mode.
//
// Example:
//
//   enum class Error {
//     GENERIC_ERROR,
//     OUT_OF_MEMORY,
//     ACCESS_DENIED,
//   };
//
//   ...
//
//   Result<Foo, Error> result = calculate();
//   if (result.ok()) {
//     result->doSomethingInteresting();
//   } else {
//     std::cerr << "Calculation failed: " << result.error() << std::endl;
//   }
//
// It is possible to use non-enumerator error code like boolean. The Result
// does not add any semantic to the value, so both true and false error codes
// in this case will be considered a non-OK result.

#pragma once

#include "astro_core/version/version.h"

#define TL_RESULT_NAMESPACE astro_core::bundled_tiny_lib::result
#include "astro_core/base/internal/tiny_lib/tl_result.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

using bundled_tiny_lib::result::Result;

}
}  // namespace astro_core
