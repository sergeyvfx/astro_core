// Copyright (c) 2019 astro core authors
//
// SPDX-License-Identifier: MIT

// Levenshtein distance - a string metric for measuring the difference between
// two sequences.

#pragma once

#include <ostream>
#include <string_view>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class LevenshteinDistance {
 public:
  LevenshteinDistance() = default;

  static auto Insertion(const int num_insertions) -> LevenshteinDistance {
    LevenshteinDistance result;
    result.num_insertions = num_insertions;
    return result;
  }
  static auto Deletion(const int num_deletions) -> LevenshteinDistance {
    LevenshteinDistance result;
    result.num_deletions = num_deletions;
    return result;
  }
  static auto Substitution(const int num_substitutions) -> LevenshteinDistance {
    LevenshteinDistance result;
    result.num_substitutions = num_substitutions;
    return result;
  }

  // Final cost of the distance: calculated as a sum of number of insertions,
  // deletions, and substitutions.
  auto GetCost() const -> int {
    return num_insertions + num_deletions + num_substitutions;
  }

  auto operator<(const LevenshteinDistance& other) const -> bool {
    return GetCost() < other.GetCost();
  }

  auto operator==(const LevenshteinDistance& other) const -> bool {
    return num_insertions == other.num_insertions &&
           num_deletions == other.num_deletions &&
           num_substitutions == other.num_substitutions;
  }
  auto operator!=(const LevenshteinDistance& other) const -> bool {
    return !(*this == other);
  }

  int num_insertions{0};
  int num_deletions{0};
  int num_substitutions{0};
};

inline auto operator+(const LevenshteinDistance& lhs,
                      const LevenshteinDistance& rhs) -> LevenshteinDistance {
  LevenshteinDistance result;
  result.num_insertions = lhs.num_insertions + rhs.num_insertions;
  result.num_deletions = lhs.num_deletions + rhs.num_deletions;
  result.num_substitutions = lhs.num_substitutions + rhs.num_substitutions;
  return result;
}

inline auto operator<<(std::ostream& os, const LevenshteinDistance& distance)
    -> std::ostream& {
  os << "num_insertions: " << distance.num_insertions;
  os << ", num_deletions: " << distance.num_deletions;
  os << ", num_substitutions: " << distance.num_substitutions;
  return os;
}

// Calculate Levenshtein distance between two strings: how many insertions,
// deletions and substitutions are to be performed on source to get target.
//
// The comparison of string happens in the case-insensitive manner for the
// primary part of thr ASCII table.
//
// The implementation is limited to strings of the maximum length of 256 to
// avoid heap allocations. Both source and target strings are trimmed to this
// length.
auto CalculateCaseInsensitiveLevenshteinDistance(std::string_view source,
                                                 std::string_view target)
    -> LevenshteinDistance;

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
