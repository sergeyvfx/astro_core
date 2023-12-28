// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

#include "astro_core/base/linked_list.h"

#include <vector>

#include "astro_core/unittest/mock.h"
#include "astro_core/unittest/test.h"

namespace astro_core {

using testing::ElementsAre;

struct IntNode {
  using ValueType = int;

  IntNode *next, *prev;

  ValueType value{0};
};

template <class Node>
auto ListElements(const LinkedList<Node>& list)
    -> std::vector<typename Node::ValueType> {
  std::vector<typename Node::ValueType> result;

  Node* node = list.GetHead();
  while (node) {
    result.push_back(node->value);
    node = node->next;
  }

  return result;
}

TEST(LinkedList, Basic) {
  LinkedList<IntNode> list;

  EXPECT_TRUE(list.IsEmpty());

  IntNode node_a{.value = 1};
  list.Append(&node_a);

  EXPECT_FALSE(list.IsEmpty());
}

TEST(LinkedList, AddAndRemove) {
  IntNode node_a{.value = 1};
  IntNode node_b{.value = 2};
  IntNode node_c{.value = 3};

  LinkedList<IntNode> list;

  list.Append(&node_a);
  list.Append(&node_b);
  list.Append(&node_c);

  EXPECT_THAT(ListElements(list), ElementsAre(1, 2, 3));

  list.Remove(&node_a);
  EXPECT_THAT(ListElements(list), ElementsAre(2, 3));

  list.Remove(&node_c);
  EXPECT_THAT(ListElements(list), ElementsAre(2));
}

}  // namespace astro_core
