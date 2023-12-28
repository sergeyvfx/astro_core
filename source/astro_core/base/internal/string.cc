// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/string.h"

#include "astro_core/base/ctype.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace {

// An implementation of std::char_traits which uses case-insensitive comparison.
struct CaseInsensitiveCharTraits : public std::char_traits<char> {
  // Compares a and b for equality.
  static constexpr auto eq(const char a, const char b) -> bool {
    return ToLowerASCII(a) == ToLowerASCII(b);
  }

  // Compares a and b in such a way that they are totally ordered.
  static constexpr auto lt(const char a, const char b) -> bool {
    return ToLowerASCII(a) < ToLowerASCII(b);
  }

  // Compares the first count characters of the character strings s1 and s2. The
  // comparison is done lexicographically.
  //
  // If count is zero, strings are considered equal.
  //
  // Returns:
  //   - Negative value if s1 is less than s2.
  //   - 00​ if s1 is equal to s2.
  //   - Positive value if s1 is greater than s2.
  static constexpr auto compare(const char* s1, const char* s2, const size_t n)
      -> int {
    size_t i = n;

    while (i-- != 0) {
      if (ToLowerASCII(*s1) < ToLowerASCII(*s2)) {
        return -1;
      }
      if (ToLowerASCII(*s1) > ToLowerASCII(*s2)) {
        return 1;
      }

      ++s1;
      ++s2;
    }

    return 0;
  }

  // Searches for character ch within the first count characters of the sequence
  // pointed to by p.
  //
  // Returns a pointer to the first character in the range specified by [p, p +
  // count) that compares equal to ch, or a null pointer if not found.
  static constexpr auto find(const char* s, const size_t n, const char a)
      -> const char* {
    size_t i = n;
    const char* ptr = s;
    while (i-- != 0) {
      if (ToLowerASCII(*ptr) == ToLowerASCII(a)) {
        return ptr;
      }
      ++ptr;
    }
    return nullptr;
  }
};

using CaseInsensitiveStringView =
    std::basic_string_view<char, CaseInsensitiveCharTraits>;

// Convert regular string view to case insensitive one.
auto MakeCaseInsensitiveStringView(const std::string_view sv)
    -> CaseInsensitiveStringView {
  return CaseInsensitiveStringView(sv.data(), sv.size());
}

}  // namespace

auto CaseInsensitiveStartsWith(const std::string_view str,
                               const std::string_view prefix) -> bool {
  return MakeCaseInsensitiveStringView(str).starts_with(
      MakeCaseInsensitiveStringView(prefix));
}

auto CaseInsensitiveFind(const std::string_view haystack,
                         const std::string_view needle) -> size_t {
  return MakeCaseInsensitiveStringView(haystack).find(
      MakeCaseInsensitiveStringView(needle));
}

auto CaseInsensitiveCompare(const std::string_view lhs,
                            const std::string_view rhs) -> int {
  return MakeCaseInsensitiveStringView(lhs).compare(
      MakeCaseInsensitiveStringView(rhs));
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
