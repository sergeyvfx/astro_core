// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/table/paged_table.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "astro_core/base/exception.h"
#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

// When it is defined to 1 the test will run tests against the std::vector.
// This is used to align the behavior of iterators and algorithms with the STL.
#define USE_STD_VECTOR_REFERENCE 0

#if USE_STD_VECTOR_REFERENCE
#  include <vector>
#endif

// Note on iterator comparison:
//
// It seems that in MSVC the order of macro evaluation somehow matches the
// function call, which makes it impossible to use checks like the following:
//
//   EXPECT_EQ(my_vector.erase(my_vector.begin() + 2), my_vector.end());
//
// The workaround is to use EXPECT_TRUE() instead. The advantage is also that
// this avoids confusing HEX memory dump of iterators when they mismatch.

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

// Wrap everything into an anonymous namespace to allow creating a local symbol
// for PagedTable when vector reference implementation is used.
namespace {

#if USE_STD_VECTOR_REFERENCE
template <class RowType, size_t kNUmRowsPerPage>
using PagedTable = std::vector<RowType>;
#endif

using testing::ElementsAre;
using testing::Eq;
using testing::Pointwise;

struct BasicRow {
  int field_a{-1};
  int field_b{-1};

  inline auto operator==(const BasicRow& other) const -> bool = default;
};

struct Row : public BasicRow {
  Row(int a, int b) : BasicRow({a, b}) {}
};

static int global_num_allocations = 0;

// STL-style allocator which keeps track of the number of allocation calls in
// the global_num_allocations global variable.
template <class T>
class CountingAllocator {
 public:
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using propagate_on_container_move_assignment = std::true_type;

  CountingAllocator() = default;

  template <class U>
  constexpr CountingAllocator(const CountingAllocator<U>&) noexcept {}

  [[nodiscard]] auto allocate(const std::size_t n) -> T* {
    if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      ThrowOrAbort<std::bad_array_new_length>();
    }

    ++global_num_allocations;

    if (void* ptr = ::operator new(n * sizeof(T))) {
      return static_cast<T*>(ptr);
    }

    ThrowOrAbort<std::bad_alloc>();
  }

  void deallocate(T* ptr, const std::size_t /*n*/) noexcept {
    ::operator delete(ptr);
  }

  template <class U>
  bool operator==(const CountingAllocator<U>& /*other*/) {
    return true;
  }
  template <class U>
  bool operator!=(const CountingAllocator<U>& /*other*/) {
    return false;
  }
};

TEST(PagedTable, BasicConstruction) {
  static int num_row_destructors_called{0};

  struct CountingRow : public BasicRow {
    ~CountingRow() { ++num_row_destructors_called; }
  };

  {
    PagedTable<CountingRow, 2> table;

    // Reserve the memory when using vector reference implementation so that we
    // do no count destructors which happen during vector grow.
#if USE_STD_VECTOR_REFERENCE
    table.reserve(3);
#endif

    table.push_back({1, 2});
    table.push_back({3, 4});
    table.push_back({5, 6});

    EXPECT_EQ(table.size(), 3);

    // The temporary rows are destroyed after they have been moved to the table.
    EXPECT_EQ(num_row_destructors_called, 3);
  }

  // The rows in the table have been destroyed.
  EXPECT_EQ(num_row_destructors_called, 6);
}

TEST(PagedTable, ElementAccess) {
  PagedTable<Row, 2> table;

  table.push_back({1, 2});
  table.push_back({3, 4});
  table.push_back({5, 6});

  EXPECT_EQ(table.front(), Row(1, 2));
  EXPECT_EQ(table.back(), Row(5, 6));

  {
    EXPECT_EQ(table.at(2), Row(5, 6));
    EXPECT_THROW_OR_ABORT(table.at(3), std::out_of_range);
  }

  { EXPECT_EQ(table[2], Row(5, 6)); }
}

TEST(PagedTable, push_back) {
  PagedTable<Row, 2> table;

  table.push_back({1, 2});  // Move.

  const Row row(3, 4);
  table.push_back(row);  // Copy

  EXPECT_EQ(table[0], Row(1, 2));
  EXPECT_EQ(table[1], Row(3, 4));
}

TEST(PagedTable, emplace_back) {
  PagedTable<Row, 2> table;

  table.emplace_back(1, 2);
  table.emplace_back(3, 4);
  table.emplace_back(5, 6);

  EXPECT_EQ(table[0], Row(1, 2));
  EXPECT_EQ(table[1], Row(3, 4));
  EXPECT_EQ(table[2], Row(5, 6));
}

TEST(PagedTable, insert) {
  PagedTable<std::string, 2> table;

  // Insert to the empty table.
  table.insert(table.begin(), "3");
  table.insert(table.begin(), "2");
  table.insert(table.begin(), "1");

  EXPECT_THAT(table, ElementsAre("1", "2", "3"));

  // Insert at the beginning.
  {
    auto it = table.insert(table.begin(), "10");
    EXPECT_EQ(*it, "10");
    EXPECT_THAT(table, Pointwise(Eq(), std::to_array({"10", "1", "2", "3"})));
  }

  // Insert at the previous to end.
  {
    auto it = table.insert(std::prev(table.end()), "20");
    EXPECT_EQ(*it, "20");
    EXPECT_THAT(table, ElementsAre("10", "1", "2", "20", "3"));
  }

  // Insert at the end.
  {
    auto it = table.insert(table.end(), "30");
    EXPECT_EQ(*it, "30");
    EXPECT_THAT(table, ElementsAre("10", "1", "2", "20", "3", "30"));
  }
}

TEST(PagedTable, emplace) {
  PagedTable<std::string, 2> table;

  // Emplace to the empty table.
  table.emplace(table.begin(), "3");
  table.emplace(table.begin(), "2");
  table.emplace(table.begin(), "1");

  EXPECT_THAT(table, ElementsAre("1", "2", "3"));

  // Emplace at the beginning.
  {
    auto it = table.emplace(table.begin(), "10");
    EXPECT_EQ(*it, "10");
    EXPECT_THAT(table, ElementsAre("10", "1", "2", "3"));
  }

  // Emplace at the previous to end.
  {
    auto it = table.emplace(std::prev(table.end()), "20");
    EXPECT_EQ(*it, "20");
    EXPECT_THAT(table, ElementsAre("10", "1", "2", "20", "3"));
  }

  // Emplace at the end.
  {
    auto it = table.emplace(table.end(), "30");
    EXPECT_EQ(*it, "30");
    EXPECT_THAT(table, ElementsAre("10", "1", "2", "20", "3", "30"));
  }
}

TEST(PagedTable, pop_back) {
  PagedTable<std::string, 2> table;

  table.push_back("foo");
  table.push_back("bar");
  table.push_back("baz");

  table.pop_back();
  table.pop_back();

  EXPECT_THAT(table, ElementsAre("foo"));
}

TEST(PagedTable, erase) {
  // Erase from the back.
  {
    PagedTable<std::string, 2> table;

    table.push_back("foo");
    table.push_back("bar");
    table.push_back("baz");

    // Store the result of erase() and compare it with an updated end().
    // Without this temporary variable the order of evaluation might not be the
    // same between different compilers.
    auto it = table.erase(table.begin() + 2);
    EXPECT_TRUE(it == table.end());

    EXPECT_THAT(table, ElementsAre("foo", "bar"));
  }

  // Erase from the middle.
  {
    PagedTable<std::string, 2> table;

    table.push_back("foo");
    table.push_back("bar");
    table.push_back("baz");

    EXPECT_TRUE(table.erase(table.begin() + 1) == table.begin() + 1);

    EXPECT_THAT(table, ElementsAre("foo", "baz"));
  }
}

TEST(PagedTable, Iterator) {
  PagedTable<Row, 2> table;

  table.push_back({1, 2});
  table.push_back({3, 4});
  table.push_back({5, 6});

  // Sequential access.
  {
    auto it = table.begin();

    EXPECT_EQ(*it, Row(1, 2));

    ++it;
    EXPECT_EQ(*it, Row(3, 4));

    ++it;
    EXPECT_EQ(*it, Row(5, 6));

    ++it;
    EXPECT_TRUE(it == table.end());
  }

  // std::prev(table.end())
  {
    auto it_prev = std::prev(table.end());
    EXPECT_EQ(*it_prev, Row(5, 6));
  }

  // std::next(std::prev(table.end()))
  {
    auto it_prev = std::prev(table.end());
    auto it_next = std::next(it_prev);
    EXPECT_TRUE(it_next == table.end());
  }

  // Prefix and postfix increment.
  {
    auto it = table.begin();
    EXPECT_EQ(*(++it), Row(3, 4));
    EXPECT_EQ(*it, Row(3, 4));

    it = table.begin();
    EXPECT_EQ(*(it++), Row(1, 2));
    EXPECT_EQ(*it, Row(3, 4));
  }

  // Increment.
  {
    EXPECT_EQ(*(table.begin() + 2), Row(5, 6));
    EXPECT_EQ(*(table.end() + (-2)), Row(3, 4));
  }

  // Decrement.
  {
    EXPECT_EQ(*(table.begin() - (-2)), Row(5, 6));
    EXPECT_EQ(*(table.end() - 2), Row(3, 4));
  }
}

TEST(PagedTable, ConstIterator) {
  // Minimal subset to check the iterator compiles with proper types.

  PagedTable<Row, 2> table;

  table.push_back({1, 2});
  table.push_back({3, 4});
  table.push_back({5, 6});

  // Sequential access.
  {
    const PagedTable<Row, 2>& const_table = table;

    auto it = const_table.begin();

    EXPECT_EQ(*it, Row(1, 2));

    ++it;
    EXPECT_EQ(*it, Row(3, 4));

    ++it;
    EXPECT_EQ(*it, Row(5, 6));

    ++it;
    EXPECT_TRUE(it == const_table.end());
  }

  // Ensure iterator can be casted to const_iterator.
  {
    PagedTable<Row, 2>::const_iterator it = table.begin();
    EXPECT_EQ(*it, Row(1, 2));
  }
}

TEST(PagedTable, RangedLoop) {
  PagedTable<Row, 2> table;

  table.push_back({1, 2});
  table.push_back({3, 4});
  table.push_back({5, 6});

  std::vector<Row> traversed_rows;
  for (Row& row : table) {
    traversed_rows.push_back(row);
  }

  EXPECT_THAT(traversed_rows,
              Pointwise(Eq(), std::to_array<Row>({{1, 2}, {3, 4}, {5, 6}})));
}

TEST(PagedTable, Sort) {
  PagedTable<Row, 2> table;

  // Fill the table with values in descending order.
  for (int i = 0; i < 10; ++i) {
    int a = 9 - i + 1;
    table.push_back({a * 2 - 1, a * 2});
  }

  std::sort(table.begin(), table.end(), [](const Row& a, const Row& b) -> bool {
    return a.field_a < b.field_a;
  });

  EXPECT_THAT(table,
              Pointwise(Eq(),
                        std::to_array<Row>({{1, 2},
                                            {3, 4},
                                            {5, 6},
                                            {7, 8},
                                            {9, 10},
                                            {11, 12},
                                            {13, 14},
                                            {15, 16},
                                            {17, 18},
                                            {19, 20}})));
}

TEST(PagedTable, Clear) {
#if !USE_STD_VECTOR_REFERENCE
  PagedTable<int, 16, CountingAllocator> table;
#else
  PagedTable<int, 16> table;
#endif

  global_num_allocations = 0;

  for (int i = 0; i < 16; ++i) {
    table.push_back(i);
  }

  table.clear();

  EXPECT_EQ(table.size(), 0);
  EXPECT_TRUE(table.empty());

  for (int i = 0; i < 16; ++i) {
    table.push_back(i);
  }

  EXPECT_THAT(
      table, ElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));

#if !USE_STD_VECTOR_REFERENCE
  EXPECT_EQ(global_num_allocations, 1);
#endif
}

////////////////////////////////////////////////////////////////////////////////
// Maintenance.

// Avoid accident of a reference test committed.
TEST(PagedTable, IsReal) { EXPECT_EQ(USE_STD_VECTOR_REFERENCE, 0); }

}  // namespace

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
