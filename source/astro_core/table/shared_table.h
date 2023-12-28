// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Design pattern which implements an efficient reader-writer access to a lookup
// table. The goal here is to allow non-blocking multi-threaded read-only access
// but also allow replacing the lookup table from another thread.
//
// The typical approach of read-write mutex is not the most efficient approach
// here because the table lookup is cheap and such mutex introduces too much of
// overhead.
//
// Example:
//
//   SharedTable<MyTable> shared_table;
//
//   // Update the table.
//   shared_table.Set(MyTable());
//
//   // Access the table from a thread.
//   const SharedTable<MyTable>::LocalTable local_table = shared_table.Load();
//   if (!local_table) {
//     raise error
//   }
//   local_table->LookupValue();

#pragma once

#include <atomic>
#include <memory>

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

template <class Table>
class SharedTable {
 public:
  using LocalTable = std::shared_ptr<const Table>;

  // Set the underlying table.
  // This call does not conflict with possible reader threads.
  void Set(Table&& table) {
    std::atomic_store(&table_, std::make_shared<Table>(std::move(table)));
  }

  // Acquire a pointer to the table.
  // The table from this pointer can be accessed in a read-only manner from
  // multiple threads.
  //
  // The result is a pointer-like object to a const Table.
  // If the table has never been set then the result object operator bool() will
  // give false.
  auto Load() const -> LocalTable { return std::atomic_load(&table_); }

 private:
  // Actual table.
  //
  // Stored as a shared pointer to allow threads to have read-only access with
  // the least amount of threading overhead, while allowing the main thread to
  /// update the table.
  //
  // TODO(sergey): Find a solution which does not require heap allocations.
  std::shared_ptr<Table> table_;
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
