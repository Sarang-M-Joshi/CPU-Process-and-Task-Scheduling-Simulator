#pragma once

#include "Maybe.h"

#include <cstddef>
#include <iostream>

namespace dsa {

class StackLinked {
    struct Node { int value; Node* next; Node(int v, Node* n) : value(v), next(n) {} };
    Node* top_{nullptr}; std::size_t size_{0};
public:
    StackLinked() = default;
    StackLinked(const StackLinked&) = delete;
    StackLinked& operator=(const StackLinked&) = delete;
    ~StackLinked() { while (top_) { Node* n = top_; top_ = top_->next; delete n; } }
    bool push(int v) { top_ = new Node(v, top_); ++size_; return true; }
    Maybe<int> pop() { if (!top_) return {}; Node* n = top_; int v = n->value; top_ = n->next; delete n; --size_; return v; }
    Maybe<int> peek() const { return top_ ? Maybe<int>{top_->value} : Maybe<int>{}; }
    void display(std::ostream& out = std::cout) const { out << "Top -> "; for (Node* n = top_; n; n = n->next) out << n->value << ' '; out << "<- Bottom\n"; }
    bool isEmpty() const noexcept { return top_ == nullptr; }
    // There is no fixed capacity; allocation failure is reported by new as std::bad_alloc.
    bool isFull() const noexcept { return false; }
    std::size_t size() const noexcept { return size_; }
};

}  // namespace dsa
