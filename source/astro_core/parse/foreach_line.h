// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parser of text data into individual lines.
//
// Handles Unix (\n), Windows (\r\n), and Classical Mac (\r) end of line
// character sequences. The new line sequence is not included into the
// strings. If there is no new line sequence at the end of the text the last
// line is still reported as parsed. None of the lines are ignored, including
// the empty lines in the middle of the text. Empty text generates no lines.
//
// Example:
//
//   for (const std::string_view line : ForeachLine("Hello\nWorld\n")) {
//     std::cout << line << std::endl;
//   }

#pragma once

#include <cstddef>
#include <iterator>
#include <string_view>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

class ForeachLine {
  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = const std::string_view;
    using pointer = value_type*;
    using reference = value_type&;

    // Constructor.

    Iterator(const std::string_view str, const size_t start_index)
        : str_(str),
          current_line_start_index_(start_index),
          next_line_start_index_(start_index) {
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
             current_line_start_index_ == other.current_line_start_index_ &&
             next_line_start_index_ == other.next_line_start_index_;
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

    inline auto operator*() const -> reference { return current_line_; }
    inline auto operator->() const -> pointer { return &current_line_; }

   private:
    // Step from the current line to the next one.
    void Step() {
      const size_t str_length = str_.size();

      // Advance to the beginning of the next line.
      current_line_start_index_ = next_line_start_index_;

      // Range checking: see whether the iterator reaches the end.
      if (current_line_start_index_ == str_length) {
        return;
      }

      // Look for the end of the line.
      size_t line_length = 0;
      while (next_line_start_index_ < str_length) {
        const char ch = str_[next_line_start_index_];

        if (ch == '\n') {
          // Unix style of EOL.
          ++next_line_start_index_;
          break;
        } else if (ch == '\r') {
          if (next_line_start_index_ < str_length - 1 &&
              str_[next_line_start_index_ + 1] == '\n') {
            // Windows style of EOL.
            ++next_line_start_index_;
          }
          // If the condition above is false, then this is the classical Mac
          // style of EOL. Otherwise, it is the end of the Windows EOL.
          ++next_line_start_index_;
          break;
        }

        ++next_line_start_index_;
        ++line_length;
      }

      current_line_ = str_.substr(current_line_start_index_, line_length);
    }

    // String which is being split into individual lines.
    std::string_view str_;

    // Start index of the current and the next line in the raw string.
    size_t current_line_start_index_{0};
    size_t next_line_start_index_{0};

    // Line at the current iterator state.
    std::string_view current_line_;
  };

 public:
  using iterator = Iterator;
  using const_iterator = Iterator;

  explicit ForeachLine(const std::string_view str) : str_(str) {}

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
