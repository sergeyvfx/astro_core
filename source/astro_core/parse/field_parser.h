// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Parsing of fields of data from text representation.
//
// For example, these utilities help parsing tables from their fixed-width text
// representation.

#pragma once

#include <string_view>
#include <type_traits>

#include "astro_core/base/result.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

// Start and end columns of a field.
struct FieldColumns {
  const size_t start;
  const size_t end;
};

// Get a view of the line which corresponds to the field denoted by its start
// and end columns. The columns of the line are base-1 indices.
//
// If the start column is past the possible line character index an empty view
// is returned.
// If the end column is past the end of line then only view up to the end of
// the line is returned.
[[nodiscard]] inline constexpr auto GetFieldView(const std::string_view line,
                                                 const size_t start_column,
                                                 const size_t end_column)
    -> std::string_view {
  const size_t field_start = start_column - 1;
  const size_t field_length = end_column - start_column + 1;

  if (field_start >= line.size()) {
    return "";
  }

  return line.substr(field_start, field_length);
}

// Parser of a generic field using the given parser.
//
// Expects that the value parser returns a Result-like object to provide
// information about error and the result value.
//
// The field is provided as a pointer to an object member.
// The columns of the line are base-1 indices. The same rules to the columns
// applies as in GetFieldView().
//
// Returns true if parsing succeeded, false otherwise.
template <class Object,
          auto Field,
          auto ValueParser,
          int kStartColumn,
          int kEndColumn>
[[nodiscard]] inline auto ParseField(Object& object,
                                     const std::string_view line) -> bool {
  std::string_view view_to_parse = GetFieldView(line, kStartColumn, kEndColumn);
  const auto result = ValueParser(view_to_parse);
  if (!result.Ok()) {
    return false;
  }

  object.*Field = result.GetValue();

  return true;
}

// Parse object field using the same value parser, but try to use multiple
// columns. The value parser is called on all requested columns until it returns
// true.
template <class Object,
          auto Field,
          auto ValueParser,
          FieldColumns kFieldColumns>
[[nodiscard]] inline auto TryParseField(Object& object,
                                        const std::string_view line) -> bool {
  return ParseField<Object,
                    Field,
                    ValueParser,
                    kFieldColumns.start,
                    kFieldColumns.end>(object, line);
}
template <class Object,
          auto Field,
          auto ValueParser,
          FieldColumns kFieldColumns,
          FieldColumns... kRemainingFieldColumns>
inline auto TryParseField(Object& object, const std::string_view line)
    -> std::enable_if_t<sizeof...(kRemainingFieldColumns) != 0, bool> {
  if (ParseField<Object,
                 Field,
                 ValueParser,
                 kFieldColumns.start,
                 kFieldColumns.end>(object, line)) {
    return true;
  }

  return TryParseField<Object, Field, ValueParser, kRemainingFieldColumns...>(
      object, line);
}

// Helper functions to run a series of parsers on the given line to
// incrementally update the state of TLE. Each of the parse iteration checks
// for a possible error and returns it when it occurs.
//
// Parser is expected to return true on success and false on failure.
template <class Object, class Parser>
inline auto RunFieldParsers(Object& object,
                            const std::string_view line,
                            Parser&& parser) -> bool {
  return parser(object, line);
}
template <class Object, class Parser, class... Parsers>
inline auto RunFieldParsers(Object& object,
                            const std::string_view line,
                            Parser&& parser,
                            Parsers... parsers) -> bool {
  if (!parser(object, line)) {
    return false;
  }

  return RunFieldParsers(object, line, parsers...);
}

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
