// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/table/shared_table.h"

#include "astro_core/unittest/test.h"

namespace astro_core {

class Table {
 public:
  int property;
};

TEST(SharedTable, Basic) {
  SharedTable<Table> shared_table;

  {
    SharedTable<Table>::LocalTable local_table = shared_table.Load();
    EXPECT_FALSE(local_table);
  }

  {
    Table table;
    table.property = 123;

    shared_table.Set(std::move(table));
  }

  {
    SharedTable<Table>::LocalTable local_table = shared_table.Load();
    EXPECT_TRUE(local_table);
    EXPECT_EQ(local_table->property, 123);
  }
}

}  // namespace astro_core
