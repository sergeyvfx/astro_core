// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Split string into words in an iterable manner.
//
// Splitting happens into words which consists of Latin alpha-numeric letters.
//
// Example:
//
//   for (const std::string_view word : ForeachWord("Hello, World!")) {
//     std::cout << word << std::endl;
//   }

#pragma once

#include <cstddef>
#include <iterator>
#include <string_view>

#include "astro_core/base/ctype.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ForeachWord {
  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const std::string_view;
    using pointer = value_type*;
    using reference = value_type&;

    // Constructor.

    Iterator(const std::string_view str, const size_t start_index)
        : str_(str),
          current_word_start_index_(start_index),
          next_word_start_index_(start_index) {
      Step();
    }

    Iterator(const Iterator& other) = default;
    Iterator(Iterator&& other) noexcept = default;

    // Assignment.

    auto operator=(const Iterator& other) -> Iterator& = default;
    auto operator=(Iterator&& other) noexcept -> Iterator& = default;

    // Comparison.

    auto operator==(const Iterator& other) const -> bool {
      return str_.data() == other.str_.data() &&
             str_.size() == other.str_.size() &&
             current_word_start_index_ == other.current_word_start_index_ &&
             next_word_start_index_ == other.next_word_start_index_;
    }
    auto operator!=(const Iterator& other) const -> bool {
      return !(*this == other);
    }

    // Advance.

    // Postfix.
    inline auto operator++(int) -> Iterator {
      Iterator current_iterator = *this;
      ++(*this);
      return current_iterator;
    }

    // Prefix.
    inline auto operator++() -> Iterator& {
      Step();
      return *this;
    }

    // Access.

    inline auto operator*() const -> reference { return current_word_; }
    inline auto operator->() const -> pointer { return &current_word_; }

   private:
    inline auto IsWordCharacter(const char ch) -> bool {
      return IsDigit(ch) || IsLatin(ch);
    }

    // Step from the current line to the next one.
    void Step() {
      const size_t str_length = str_.size();

      // Advance to the beginning of the next line.
      current_word_start_index_ = next_word_start_index_;

      // Range checking: see whether the iterator reaches the end.
      if (current_word_start_index_ == str_length) {
        return;
      }

      // Skip whitespace.
      while (current_word_start_index_ < str_length &&
             !IsWordCharacter(str_[current_word_start_index_])) {
        ++current_word_start_index_;
      }

      // Collect word.
      next_word_start_index_ = current_word_start_index_;
      size_t word_length = 0;
      while (next_word_start_index_ < str_length &&
             IsWordCharacter(str_[next_word_start_index_])) {
        ++next_word_start_index_;
        ++word_length;
      }

      current_word_ = str_.substr(current_word_start_index_, word_length);
    }

    // String which is being split into individual word.
    std::string_view str_;

    // Start index of the current and the next word in the raw string.
    size_t current_word_start_index_{0};
    size_t next_word_start_index_{0};

    // Word at the current iterator state.
    std::string_view current_word_;
  };

 public:
  using iterator = Iterator;
  using const_iterator = Iterator;

  explicit ForeachWord(const std::string_view str) : str_(str) {}

  inline auto begin() -> iterator { return iterator(str_, 0); }
  inline auto begin() const -> const_iterator {
    return const_iterator(str_, 0);
  }

  inline auto end() -> iterator { return iterator(str_, str_.size()); }
  inline auto end() const -> const_iterator {
    return const_iterator(str_, str_.size());
  }

 private:
  std::string_view str_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
