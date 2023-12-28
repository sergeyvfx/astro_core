// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// A storage for table which stores data in pages of a fixed size. The pages are
// organized in a list structure.
//
// On an API level this storage is very close to std::vector<>: it supports
// random access iterator, which makes it possible to use algorithms from the
// standard library. The benefit of using this data structure over the vector
// is that when the table needs to grow it does not need to re-allocate the
// entire storage.
//
// Optimized for:
//
//   - One-time sequential creation of many rows.
//
//   - Re-creation from scratch.
//
// Not optimized for:
//
//   - Removal and insertion of rows.
//
//   - Random element access.
//
//   NOTE: While the table is not optimized for those operations, it does not
//   mean they are impossible. It only means that the computational complexity
//   is not very good.
//
// NOTE: This is an experimental API, it might get changed in the future.

#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

#include "astro_core/base/exception.h"
#include "astro_core/base/linked_list.h"
#include "astro_core/base/static_vector.h"
#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

namespace experimental {

template <class RowType,
          size_t kNumRowsPerPage,
          template <class> class Allocator = std::allocator>
class PagedTable {
  //////////////////////////////////////////////////////////////////////////////
  // Page.

  class Page : public StaticVector<RowType, kNumRowsPerPage> {
   public:
    // Links to the next and previous pages in the list.
    Page *next{nullptr}, *prev{nullptr};

    // Returns true when the page and no new rows can be added to it.
    auto full() const -> bool { return this->size() == this->capacity(); }
  };

  //////////////////////////////////////////////////////////////////////////////
  // Iterator implementation.
  //
  // Templated to de-duplicate implementation for regular and constant iterator.
  //
  // The iterator is provided with the page and row index within the page to the
  // element it corresponds to, as well as the global index within the table.
  //
  // Having the global index makes it possible to compare two iterators.
  // having page and index with the page allows for a quick access of the row
  // from the iterator.
  //
  // The end iterator is expected to be the last page with a row index past the
  // actual rows of the page. This allows to go to the previous iterator from
  // the end iterator. As in, `std::prev(table.end())` will point to the same
  // element as `table.back()`.
  template <class ValueType>
  class Iterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = ValueType;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = ptrdiff_t;

    ////////////////////////////////////////////////////////////////////////////
    // Constructor.

    Iterator(Page* page,
             const size_t page_row_index,
             const size_t table_row_index)
        : page_(page),
          page_row_index_(page_row_index),
          table_row_index_(table_row_index) {}

    Iterator(const Iterator& other) = default;
    Iterator(Iterator&& other) noexcept = default;

    // Special constructor of const_iterator from iterator.
    template <class OtherValueType,
              class = std::enable_if_t<
                  std::is_const_v<ValueType> &&
                  std::is_same_v<OtherValueType, std::remove_cv_t<ValueType>>>>
    Iterator(const Iterator<OtherValueType>& other)
        : page_(other.page_),
          page_row_index_(other.page_row_index_),
          table_row_index_(other.table_row_index_) {}

    ////////////////////////////////////////////////////////////////////////////
    // Assignment.

    auto operator=(const Iterator& other) -> Iterator& = default;
    auto operator=(Iterator&& other) noexcept -> Iterator& = default;

    ////////////////////////////////////////////////////////////////////////////
    // Comparison.

    auto operator==(const Iterator<std::remove_const_t<ValueType>>& other)
        const noexcept -> bool {
      return table_row_index_ == other.table_row_index_;
    }
    auto operator==(const Iterator<const std::remove_const_t<ValueType>>& other)
        const noexcept -> bool {
      return table_row_index_ == other.table_row_index_;
    }
    auto operator!=(const Iterator<std::remove_const_t<ValueType>>& other)
        const noexcept -> bool {
      return !(*this == other);
    }
    auto operator!=(const Iterator<const std::remove_const_t<ValueType>>& other)
        const noexcept -> bool {
      return !(*this == other);
    }

    auto operator<=>(
        const Iterator<std::remove_const_t<ValueType>>& other) const noexcept {
      return table_row_index_ <=> other.table_row_index_;
    }
    auto operator<=>(const Iterator<const std::remove_const_t<ValueType>>&
                         other) const noexcept {
      return table_row_index_ <=> other.table_row_index_;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Advance.

    // Postfix.
    inline auto operator++(int) -> Iterator {
      Iterator current_iterator = *this;
      ++(*this);
      return current_iterator;
    }

    // Prefix.
    inline auto operator++() -> Iterator {
      assert(page_ != nullptr);

      ++page_row_index_;
      ++table_row_index_;

      // Wrap to the next page, unless it is the last page.
      if (page_row_index_ == page_->size() && page_->next) {
        page_ = page_->next;
        page_row_index_ = 0;
      }

      return *this;
    }

    // Postfix.
    inline auto operator--(int) -> Iterator {
      Iterator current_iterator = *this;
      --(*this);
      return current_iterator;
    }

    // Prefix.
    inline auto operator--() -> Iterator {
      assert(page_ != nullptr);

      if (page_row_index_ == 0) {
        page_ = page_->prev;
        page_row_index_ = page_->size();
      }

      --page_row_index_;
      --table_row_index_;

      return *this;
    }

    friend inline auto operator+(const Iterator& lhs, const difference_type rhs)
        -> Iterator {
      Iterator result = lhs;
      result += rhs;
      return result;
    }
    inline auto operator+=(const difference_type rhs) -> Iterator& {
      if (rhs > 0) {
        AdvanceForward(rhs);
      } else if (rhs < 0) {
        AdvanceBackward(-rhs);
      }

      return *this;
    }

    friend inline auto operator-(const Iterator& lhs, const difference_type rhs)
        -> Iterator {
      Iterator result = lhs;
      result -= rhs;
      return result;
    }
    inline auto operator-=(const difference_type rhs) -> Iterator& {
      if (rhs > 0) {
        AdvanceBackward(rhs);
      } else if (rhs < 0) {
        AdvanceForward(-rhs);
      }

      return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Difference.

    friend inline auto operator-(const Iterator& lhs, const Iterator& rhs)
        -> difference_type {
      return difference_type(lhs.table_row_index_) -
             difference_type(rhs.table_row_index_);
    }

    ////////////////////////////////////////////////////////////////////////////
    // Access.

    inline auto operator*() const -> reference {
      return (*page_)[page_row_index_];
    }
    inline auto operator->() const -> pointer {
      return &(*page_)[page_row_index_];
    }

   private:
    friend class Iterator<std::remove_const_t<ValueType>>;
    friend class Iterator<const std::remove_const_t<ValueType>>;

    // Mark const iterator as friend, so it can access fields of non-const
    // iterator in constructor.
    friend Iterator<const ValueType>;

    void AdvanceForward(const difference_type count) {
      assert(page_ != nullptr);

      // Advance both indices.
      page_row_index_ += count;
      table_row_index_ += count;

      // Wrap to the next pages util the page index is within the page size.
      // Don't loop past the last page. This way advancing will move from the
      // last element to the proper end() marker.
      while (page_row_index_ >= page_->size() && page_->next) {
        page_row_index_ -= page_->size();
        page_ = page_->next;
      }
    }

    void AdvanceBackward(const difference_type count) {
      while (count > page_row_index_) {
        page_ = page_->prev;
        assert(page_ != nullptr);

        const size_t page_size = page_->size();
        page_row_index_ += page_size;
      }

      page_row_index_ -= count;
      table_row_index_ -= count;
    }

    // Page and the index within the page which corresponds to this iterator.
    Page* page_{nullptr};
    size_t page_row_index_{0};

    // The index of the row within the table.
    size_t table_row_index_{0};
  };

 public:
  using value_type = RowType;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = Iterator<RowType>;
  using const_iterator = Iterator<const RowType>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  //////////////////////////////////////////////////////////////////////////////
  // Constants.

  // In-class alias for the number of rows per page.
  static constexpr size_type num_rows_per_page = kNumRowsPerPage;

  //////////////////////////////////////////////////////////////////////////////
  // Constructor and assignment.

  PagedTable() = default;

  ~PagedTable() {
    FreePageList(pages_);
    DeallocatePageList(allocated_pages_);
  }

  // Copy constructor and assignment is not impossible to implement, but is
  // currently not supported.
  PagedTable(const PagedTable& other) = delete;
  auto operator=(const PagedTable& other) -> PagedTable& = delete;

  PagedTable(PagedTable&& other) noexcept
      : pages_(other.pages_), num_rows_(other.num_rows_) {
    other.pages_.Clear();
    other.num_rows_ = 0;
  }

  auto operator=(PagedTable&& other) -> PagedTable& {
    if (this == &other) {
      return *this;
    }

    pages_ = other.pages_;
    num_rows_ = other.num_rows_;

    other.pages_.Clear();
    other.num_rows_ = 0;

    return *this;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Element access.

  // Returns a reference to the row at specified index.
  //
  // Bounds checking is performed, exception of type std::out_of_range will be
  // thrown on invalid access.
  constexpr auto at(const size_t index) -> RowType& {
    if (index >= size()) {
      ThrowOrAbort<std::out_of_range>("index >= size()");
    }
    return *(begin() + index);
  }
  constexpr auto at(const size_t index) const -> const RowType& {
    if (index >= size()) {
      ThrowOrAbort<std::out_of_range>("index >= size()");
    }
    return *(begin() + index);
  }

  // Returns a reference to the row at specified index.
  // No bounds checking is performed. If pos >= size(), the behavior is
  constexpr auto operator[](const size_t index) -> reference {
    return *(begin() + index);
  }
  constexpr auto operator[](const size_t index) const -> const_reference {
    return *(begin() + index);
  }

  // Access the front (the oldest) element of the buffer.
  //
  // Calling front on an empty buffer is undefined.
  inline auto front() -> RowType& {
    assert(!empty());
    return GetHeadPage()->front();
  }
  inline auto front() const -> const RowType& {
    assert(!empty());
    return GetHeadPage()->front();
  }

  // Access the back (the newest) element of the buffer.
  //
  // Calling back on an empty buffer is undefined.
  inline auto back() -> RowType& {
    assert(!empty());
    return GetTailPage()->back();
  }
  inline auto back() const -> const RowType& {
    assert(!empty());
    return GetTailPage()->back();
  }

  //////////////////////////////////////////////////////////////////////////////
  // Iterators.

  // Returns an iterator to the first row of the table.
  // begin() returns a mutable or constant iterator, depending on the constness
  // of *this.
  // cbegin() always returns a constant iterator. It is equivalent to
  // const_cast<const PagedTable&>(*this).begin().
  inline auto begin() -> iterator { return iterator(GetHeadPage(), 0, 0); }
  inline auto begin() const -> const_iterator {
    return const_iterator(GetHeadPage(), 0, 0);
  }
  constexpr auto cbegin() const noexcept -> const_iterator { return begin(); }

  // Returns an iterator to the row following the last row of the table.
  inline auto end() -> iterator {
    Page* tail_page = GetTailPage();
    return iterator(tail_page, tail_page ? tail_page->size() : 0, size());
  }
  inline auto end() const -> const_iterator {
    Page* tail_page = GetTailPage();
    return const_iterator(tail_page, tail_page ? tail_page->size() : 0, size());
  }
  constexpr auto cend() const noexcept -> const_iterator { return end(); }

  // Returns a reverse iterator to the row of the reversed table.
  // It corresponds to the last row of the non-reversed table.
  constexpr auto rbegin() noexcept -> reverse_iterator {
    return reverse_iterator{end()};
  }
  constexpr auto rbegin() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{cend()};
  }
  constexpr auto crbegin() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{cend()};
  }

  // Returns a reverse iterator to the row following the last row of the
  // reversed string. It corresponds to the row preceding the first row of the
  // non-reversed table.
  // This row acts as a placeholder, attempting to access it results in
  // undefined behavior.
  constexpr auto rend() noexcept -> reverse_iterator {
    return reverse_iterator{begin()};
  }
  constexpr auto rend() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{cbegin()};
  }
  constexpr auto crend() const noexcept -> const_reverse_iterator {
    return const_reverse_iterator{cbegin()};
  }

  //////////////////////////////////////////////////////////////////////////////
  // Capacity.

  // Returns the current number of elements.
  inline auto size() const -> size_type { return num_rows_; }

  // Check whether the container is empty.
  inline auto empty() const -> bool { return size() == 0; }

  //////////////////////////////////////////////////////////////////////////////
  // Modification.

  // Clears the contents of the table.
  void clear() {
    Page* page = GetHeadPage();
    while (page != nullptr) {
      Page* next_page = page->next;

      std::destroy_at(page);
      allocated_pages_.Append(page);

      page = next_page;
    }
    pages_.Clear();

    num_rows_ = 0;
  }

  // Appends the given row value to the end of the container.
  void push_back(const RowType& row) {
    Page* page = GetPageForPushBack();
    page->push_back(row);
    ++num_rows_;
  }
  void push_back(RowType&& row) {
    Page* page = GetPageForPushBack();
    page->push_back(std::move(row));
    ++num_rows_;
  }

  // Appends a new row to the end of the container.
  // The row is constructed through in-place constructor initialization called
  // with the given arguments.
  template <class... Args>
  constexpr auto emplace_back(Args&&... args) -> RowType& {
    Page* page = GetPageForPushBack();
    RowType& row = page->emplace_back(std::forward<Args>(args)...);
    ++num_rows_;
    return row;
  }

  // Insert row directly before the pos.
  auto insert(const const_iterator pos, const RowType& row) -> iterator {
    if (pos == end()) {
      push_back(row);
      return std::prev(end());
    }

    iterator insert_pos = MoveRowsBack(pos);
    *insert_pos = row;
    return insert_pos;
  }

  // Insert row directly before the pos.
  // The row is constructed through in-place constructor initialization called
  // with the given arguments.
  template <class... Args>
  auto emplace(const const_iterator pos, Args&&... args) -> iterator {
    if (pos == end()) {
      emplace_back(RowType(std::forward<Args>(args)...));
      return std::prev(end());
    }

    iterator insert_pos = MoveRowsBack(pos);
    *insert_pos = std::move(RowType(std::forward<Args>(args)...));
    return insert_pos;
  }

  // Removes the last element of the container.
  // Calling pop_back on an empty container results in undefined behavior.
  void pop_back() {
    assert(!empty());

    // Remove element from the last page.
    Page* last_page = GetTailPage();
    last_page->pop_back();

    // If he [age is fully empty, remove it from the list of used pages and
    // store it for later memory re-sue.
    if (last_page->empty()) {
      pages_.Remove(last_page);
      allocated_pages_.Append(last_page);
    }

    --num_rows_;
  }

  // Removes the element at pos.
  auto erase(const const_iterator pos) -> iterator {
    if (pos == std::prev(end())) {
      pop_back();
      return end();
    }

    return MoveRowsForward(pos);
  }

 private:
  //////////////////////////////////////////////////////////////////////////////
  // Internal implementation.

  // The list of pages.
  using PageList = LinkedList<Page>;

  // Allocate new page.
  //
  // The page has pre-allocated storage for the kNumRowsPerPage which is not
  // initialize.
  //
  // The page is added at the end of the pages which form the data of this
  // table.
  auto AllocatePage() -> Page* {
    Page* page_memory = nullptr;

    if (!allocated_pages_.IsEmpty()) {
      page_memory = allocated_pages_.GetHead();
      allocated_pages_.Remove(page_memory);
    } else {
      page_memory = page_allocator_.allocate(1);
    }

    Page* page = new (page_memory) Page();
    pages_.Append(page);
    return page;
  }

  // Free all pages in the list, leaving the list in a valid empty state.
  // Destroys the page and deallocates its memory.
  void FreePageList(PageList& pages) {
    Page* page = pages.GetHead();
    while (page != nullptr) {
      Page* next_page = page->next;

      std::destroy_at(page);
      page_allocator_.deallocate(page, 1);

      page = next_page;
    }
    pages.Clear();
  }

  // Deallocate pages on the list, without running destructor on them.
  void DeallocatePageList(PageList& pages) {
    Page* page = pages.GetHead();
    while (page != nullptr) {
      Page* next_page = page->next;

      page_allocator_.deallocate(page, 1);

      page = next_page;
    }
    pages.Clear();
  }

  // Get page which is at the head and tail of the pages list.
  auto GetHeadPage() const -> Page* { return pages_.GetHead(); }
  auto GetTailPage() const -> Page* { return pages_.GetTail(); }

  // Get page for emplace_back and push_back type of operations.
  // Will create a new page when needed.
  auto GetPageForPushBack() -> Page* {
    Page* page = GetTailPage();
    if (page == nullptr || page->full()) {
      page = AllocatePage();
    }
    return page;
  }

  // Move the rows one position to the end of the table starting from the given
  // position. This will increase the size of the table, and leave the row at
  // the position pos in undefined but valid state.
  auto MoveRowsBack(const const_iterator pos) -> iterator {
    // Make sure there is a page to move the current last row to.
    Page* last_page = GetPageForPushBack();

    // Emplace the currently last row to the end of the table.
    last_page->emplace_back(std::move(*std::prev(end())));
    ++num_rows_;

    // Move the rest of the rows one position forward.
    iterator dst_it = std::prev(end(), 2);
    while (dst_it != pos) {
      const iterator src_it = std::prev(dst_it);

      *dst_it = std::move(*src_it);

      dst_it = src_it;
    }

    return dst_it;
  }

  // Move the rows one position to the beginning of the table starting from the
  // given position. This will decrease the size of the table, leaving the
  // emptied row in a destroyed and non-initialized state.
  auto MoveRowsForward(const const_iterator pos) -> iterator {
    // TODO(sergey): In theory it is possible to avoid looping through the
    // iterator advance.
    iterator start_it = begin() + std::distance(cbegin(), pos);

    const iterator end_it = end();

    // Shift elements one position to the beginning.
    iterator dst_it = start_it;
    iterator src_it = dst_it + 1;
    while (src_it != end_it) {
      *dst_it = std::move(*src_it);

      ++dst_it;
      ++src_it;
    }

    // Pop the last element which is left in a moved-from state.
    // This will take care of page re-use check, and change in the actual table
    // size.
    pop_back();

    return start_it;
  }

  // Pages of the table.
  // This is where the actual content is stored.
  PageList pages_;

  // Pages which have been allocated but later destroyed.
  // This list contains memory which points to an uninitialized page objects.
  PageList allocated_pages_;

  // The number of rows in the table.
  size_t num_rows_{0};

  Allocator<Page> page_allocator_;
};

}  // namespace experimental

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
