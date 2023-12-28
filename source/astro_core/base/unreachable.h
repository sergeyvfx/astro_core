// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Utility helper to mark code as unreachable in a well-defined manner.
//
// Consider the example:
//
//   const enum Number {
//     kZero,
//     kOne,
//   };
//
//   auto MyFunction(const Number number) -> const char* {
//     switch (number) {
//       case Number::kZero: return "Cat";
//       case Number::kOne: return "Dog";
//     }
//   }
//
// The code after the switch statement is not meant to be ever executed because
// all enumerator cases are expected to be handled. Most of the compilers have
// a warning which will inform that an enumerator value is not handled in the
// switch statement which will help finding cases where something is not
// handled. Such warning only works when there is no `default` case in the
// switch statement. But sometimes compilers might still warn about function
// not retuning a value. Having an explicit call of a function which is marked
// as `[[noreturn]]` will stop the warning.
//
// It is also important to inform developer (or a user) that unreachable code
// was actually hit. The fact that no meaningful return value was given by a
// developer suggests that this isn't really a recoverable situation which can
// be treated as FATAL in both release and debug builds.
//
// This is exactly what `UNREACHABLE()` function is taking care of:
//
//  - Minimized a boiler plate code of error handling caused by possible
//    refactor (so that unexpected code is marked as such with a single function
//    call).
//
//  - Allows to not have `default` case which simplifies refactors.
//
//  - Silences strict compiler warnings.
//
//  - Has a defined behavior in both debug and release builds.
//
// The example above becomes:
//
//   auto MyFunction(const Number number) -> const char*{
//     switch (number) {
//       case Number::kZero: return "Cat";
//       case Number::kOne: return "Dog";
//     }
//     UNREACHABLE();
//   }
//
// NOTE: As was mentioned above the unreachable code is considered a fatal
// unrecoverable error. If in some code it is more desirable to use some
// fallback return value use that instead of `UNREACHABLE()`.

#pragma once

#include <cstdio>
#include <cstdlib>

#include "astro_core/base/source_location.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace unreachable_intern {

[[noreturn]] inline void UnreachableAbort() { abort(); }

[[noreturn]] inline void UnreachableImpl(const char* function,
                                         const char* file,
                                         const int line) {
  // Extract a base file name from the full file name.
  // Done for possible safety reasons to not expose build environment setup.

  const char* last_char = nullptr;
  for (size_t i = 0; file[i] != '\0'; ++i) {
    if (file[i] == '/' || file[i] == '\\') {
      last_char = file + i;
    }
  }

  const char* base_file_name = last_char ? last_char + 1 : file;

  // Inform developer or user of the application that unrecoverable situation
  // happened.
  //
  // TODO(sergey): Think of some better integration for embedded systems where
  // there could be no support of printing to the `stderr`. Maybe use some weak
  // linked function which can be overwritten on the embedded application side?
  ::fprintf(stderr,
            "Unreachable code was reached in function %s, file %s, line %d. "
            "No recovery possible, aborting.\n",
            function,
            base_file_name,
            line);
  ::fflush(stderr);

  // TODO(sergey): Provide backtrace.

  UnreachableAbort();
}

}  // namespace unreachable_intern

[[noreturn]] inline void UNREACHABLE(
    const source_location location = source_location::current()) {
  unreachable_intern::UnreachableImpl(
      location.function_name(), location.file_name(), location.line());
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
