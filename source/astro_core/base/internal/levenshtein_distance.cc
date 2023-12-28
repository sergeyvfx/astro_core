// Copyright (c) 2019 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/levenshtein_distance.h"

#include <algorithm>
#include <array>

#include "astro_core/base/algorithm.h"
#include "astro_core/base/ctype.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

constexpr size_t kMaxStaticStringLength = 256;

}

// Implementation is based on code from
//
//   https://en.wikipedia.org/wiki/Levenshtein_distance
//
// Modified to keep track of insertions, deletions, and substitutions.
LevenshteinDistance CalculateCaseInsensitiveLevenshteinDistance(
    const std::string_view source, const std::string_view target) {
  const size_t source_length = Min(source.length(), kMaxStaticStringLength);
  const size_t target_length = Min(target.length(), kMaxStaticStringLength);

  // Simple case: empty strings.
  if (source_length == 0) {
    return LevenshteinDistance::Insertion(target_length);
  }
  if (target_length == 0) {
    return LevenshteinDistance::Deletion(source_length);
  }

  // Create two work arrays of integer distances.
  std::array<LevenshteinDistance, kMaxStaticStringLength> v0;
  std::array<LevenshteinDistance, kMaxStaticStringLength> v1;

  // Initialize v0 (the previous row of distances).
  // This row is A[0][i]: edit distance for an empty source.
  // The distance is just the number of characters to insert into source to get
  // target.
  for (size_t i = 0; i <= target_length; ++i) {
    v0[i] = LevenshteinDistance::Insertion(i);
  }

  for (size_t i = 0; i < source_length; ++i) {
    // Calculate v1 (current row distances) from the previous row v0.

    // First element of v1 is A[i+1][0].
    // Edit distance is delete (i+1) chars from source to match empty target.
    v1[0] = LevenshteinDistance::Deletion(i + 1);

    // Use formula to fill in the rest of the row.
    for (int j = 0; j < target_length; ++j) {
      // Calculating costs for A[i+1][j+1].
      LevenshteinDistance deletion_cost =
          v0[j + 1] + LevenshteinDistance::Deletion(1);
      LevenshteinDistance insertion_cost =
          v1[j] + LevenshteinDistance::Insertion(1);
      LevenshteinDistance substitution_cost;
      if (ToLowerASCII(source[i]) == ToLowerASCII(target[j])) {
        substitution_cost = v0[j];
      } else {
        substitution_cost = v0[j] + LevenshteinDistance::Substitution(1);
      }
      v1[j + 1] =
          std::min(deletion_cost, std::min(insertion_cost, substitution_cost));
    }

    // Copy v1 (current row) to v0 (previous row) for next iteration.
    v0.swap(v1);
  }

  // After the last swap, the results of v1 are now in v0.
  return v0[target_length];
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
