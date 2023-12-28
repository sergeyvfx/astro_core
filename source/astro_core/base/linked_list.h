// Copyright (c) 2022 astro core authors
//
// SPDX-License-Identifier: MIT

// Simple implementation of a double-linked list which uses nodes of pre-defined
// type. There is no allocation happening in this list.
//
// The node is expected to contain `Node* prev` and `Node* next` fields.
// The node can only belong to single linked list at a time.

#pragma once

#include "astro_core/version/version.h"

namespace astro_core {
inline namespace ASTRO_CORE_VERSION_NAMESPACE {

template <class Node>
class LinkedList {
 public:
  // Clear the list.
  // Does not perform any operation on the nodes of the list.
  void Clear() { head_ = tail_ = nullptr; }

  // Check whether the list is empty.
  auto IsEmpty() const -> bool { return head_ == nullptr; }

  // Append given node to the end of the list.
  // If the link belongs to this or another list then the behavior is undefined.
  void Append(Node* node) {
    if (tail_ != nullptr) {
      tail_->next = node;
    }

    node->prev = tail_;
    node->next = nullptr;

    tail_ = node;
    if (head_ == nullptr) {
      head_ = node;
    }
  }

  // Remove node from the list.
  // If the node does not belong to this list the behavior is undefined.
  void Remove(Node* node) {
    if (node->prev) {
      node->prev->next = node->next;
    }
    if (node->next) {
      node->next->prev = node->prev;
    }

    if (node == head_) {
      head_ = node->next;
    }
    if (node == tail_) {
      tail_ = node->prev;
    }
  }

  auto GetHead() const -> Node* { return head_; }
  auto GetTail() const -> Node* { return tail_; }

 private:
  // Pointers to the first and last nodes of the list, respectively.
  Node* head_{nullptr};
  Node* tail_{nullptr};
};

}  // namespace ASTRO_CORE_VERSION_NAMESPACE
}  // namespace astro_core
