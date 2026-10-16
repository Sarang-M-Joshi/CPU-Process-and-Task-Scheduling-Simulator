#pragma once

#include "Maybe.h"

#include <cstddef>
#include <iostream>
#include <utility>

namespace dsa {

class DLL {
    struct Node { int value; Node* prev{nullptr}; Node* next{nullptr}; explicit Node(int v) : value(v) {} };
    Node* head_{nullptr}; Node* tail_{nullptr}; std::size_t size_{0};
    Node* nodeAt(std::size_t i) const {
        if (i < size_ / 2) { Node* n = head_; while (i--) n = n->next; return n; }
        Node* n = tail_; for (std::size_t j = size_ - 1; j > i; --j) n = n->prev; return n;
    }
public:
    DLL() = default;
    DLL(const DLL& other) {
        try { for (Node* n = other.head_; n; n = n->next) insertEnd(n->value); }
        catch (...) { clear(); throw; }
    }
    DLL& operator=(const DLL& other) { if (this != &other) { DLL copy(other); swap(copy); } return *this; }
    DLL(DLL&& o) noexcept : head_(o.head_), tail_(o.tail_), size_(o.size_) { o.head_ = o.tail_ = nullptr; o.size_ = 0; }
    DLL& operator=(DLL&& o) noexcept { if (this != &o) { clear(); head_ = o.head_; tail_ = o.tail_; size_ = o.size_; o.head_ = o.tail_ = nullptr; o.size_ = 0; } return *this; }
    ~DLL() { clear(); }
    void swap(DLL& o) noexcept { std::swap(head_, o.head_); std::swap(tail_, o.tail_); std::swap(size_, o.size_); }
    void clear() noexcept { while (head_) { Node* n = head_; head_ = head_->next; delete n; } tail_ = nullptr; size_ = 0; }
    bool isEmpty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }
    bool insertBeginning(int v) { Node* n = new Node(v); n->next = head_; if (head_) head_->prev = n; else tail_ = n; head_ = n; ++size_; return true; }
    bool insertEnd(int v) { Node* n = new Node(v); n->prev = tail_; if (tail_) tail_->next = n; else head_ = n; tail_ = n; ++size_; return true; }
    bool insertAt(std::size_t i, int v) { if (i > size_) return false; if (!i) return insertBeginning(v); if (i == size_) return insertEnd(v); Node* at = nodeAt(i); Node* n = new Node(v); n->prev = at->prev; n->next = at; at->prev->next = n; at->prev = n; ++size_; return true; }
    Maybe<int> deleteBeginning() { return size_ ? deleteAt(0) : Maybe<int>{}; }
    Maybe<int> deleteEnd() { return size_ ? deleteAt(size_ - 1) : Maybe<int>{}; }
    Maybe<int> deleteAt(std::size_t i) { if (i >= size_) return {}; Node* n = nodeAt(i); int v = n->value; if (n->prev) n->prev->next = n->next; else head_ = n->next; if (n->next) n->next->prev = n->prev; else tail_ = n->prev; delete n; --size_; return v; }
    Maybe<std::size_t> search(int v) const { std::size_t i = 0; for (Node* n = head_; n; n = n->next, ++i) if (n->value == v) return i; return {}; }
    bool update(std::size_t i, int v) { if (i >= size_) return false; nodeAt(i)->value = v; return true; }
    void reverse() noexcept { Node* n = head_; while (n) { std::swap(n->next, n->prev); n = n->prev; } std::swap(head_, tail_); }
    void displayForward(std::ostream& out = std::cout) const { out << '['; for (Node* n = head_; n; n = n->next) { if (n != head_) out << " <-> "; out << n->value; } out << "]\n"; }
    void displayBackward(std::ostream& out = std::cout) const { out << '['; for (Node* n = tail_; n; n = n->prev) { if (n != tail_) out << " <-> "; out << n->value; } out << "]\n"; }
};

}  // namespace dsa
