// Copyright (c) 2023 astro core authors
//
// SPDX-License-Identifier: MIT

// Semantic version of the Astro Core library.
#define ASTRO_CORE_VERSION_MAJOR 0
#define ASTRO_CORE_VERSION_MINOR 3
#define ASTRO_CORE_VERSION_REVISION 0

// Release cycle of the Astro Core library:
//
// - dev: the release version has been increased from the previous release, but
//   the API is not yet stabilized. New functionality and fixes happens without
//   increasing the library version.
#define ASTRO_CORE_RELEASE_CYCLE dev

// Helpers for ASTRO_CORE_VERSION_NAMESPACE.
//
// Typical extra indirection for such conversion to allow macro to be expanded
// before it is converted to string.
#define ASTRO_CORE_VERSION_NAMESPACE_CONCAT_HELPER(id1, id2, id3)              \
  v_##id1##_##id2##_##id3
#define ASTRO_CORE_VERSION_NAMESPACE_CONCAT(id1, id2, id3)                     \
  ASTRO_CORE_VERSION_NAMESPACE_CONCAT_HELPER(id1, id2, id3)

// Constructs identifier suitable for namespace denoting the current library
// version.
//
// For example: ASTRO_CORE_VERSION_NAMESPACE -> v_0_1_9
#define ASTRO_CORE_VERSION_NAMESPACE                                           \
  ASTRO_CORE_VERSION_NAMESPACE_CONCAT(ASTRO_CORE_VERSION_MAJOR,                \
                                      ASTRO_CORE_VERSION_MINOR,                \
                                      ASTRO_CORE_VERSION_REVISION)
