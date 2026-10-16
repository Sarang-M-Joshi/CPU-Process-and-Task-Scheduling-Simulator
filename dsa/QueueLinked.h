#pragma once

#include "Maybe.h"

#include <cstddef>
#include <iostream>

namespace dsa {

class QueueLinked {
    struct Node { int value; Node* next{nullptr}; explicit Node(int v) : value(v) {} };
    Node* front_{nullptr}; Node* rear_{nullptr}; std::size_t size_{0};
public:
    QueueLinked() = default;
    QueueLinked(const QueueLinked&) = delete;
    QueueLinked& operator=(const QueueLinked&) = delete;
    ~QueueLinked() { while (front_) { Node* n = front_; front_ = front_->next; delete n; } rear_ = nullptr; }
    bool enqueue(int v) { Node* n = new Node(v); if (rear_) rear_->next = n; else front_ = n; rear_ = n; ++size_; return true; }
    Maybe<int> dequeue() { if (!front_) return {}; Node* n = front_; int v = n->value; front_ = n->next; delete n; --size_; if (!front_) rear_ = nullptr; return v; }
    Maybe<int> front() const { return front_ ? Maybe<int>{front_->value} : Maybe<int>{}; }
    void display(std::ostream& out = std::cout) const { out << "Front -> "; for (Node* n = front_; n; n = n->next) out << n->value << ' '; out << "<- Rear\n"; }
    bool isEmpty() const noexcept { return front_ == nullptr; }
    // No capacity is configured; resource exhaustion is reported by new as std::bad_alloc.
    bool isFull() const noexcept { return false; }
    std::size_t size() const noexcept { return size_; }
};

}  // namespace dsa
