#pragma once

#include "Maybe.h"

#include <cstddef>
#include <iostream>
#include <utility>

namespace dsa {

class SLL {
    struct Node { int value; Node* next; explicit Node(int v) : value(v), next(nullptr) {} };
    Node* head_{nullptr};
    std::size_t size_{0};

    Node* nodeAt(std::size_t index) const {
        Node* node = head_;
        while (index-- > 0 && node) node = node->next;
        return node;
    }
public:
    SLL() = default;
    SLL(const SLL& other) {
        try { for (Node* n = other.head_; n; n = n->next) insertEnd(n->value); }
        catch (...) { clear(); throw; }
    }
    SLL& operator=(const SLL& other) { if (this != &other) { SLL copy(other); swap(copy); } return *this; }
    SLL(SLL&& other) noexcept : head_(other.head_), size_(other.size_) { other.head_ = nullptr; other.size_ = 0; }
    SLL& operator=(SLL&& other) noexcept { if (this != &other) { clear(); head_ = other.head_; size_ = other.size_; other.head_ = nullptr; other.size_ = 0; } return *this; }
    ~SLL() { clear(); }
    void swap(SLL& other) noexcept { std::swap(head_, other.head_); std::swap(size_, other.size_); }
    void clear() noexcept { while (head_) { Node* old = head_; head_ = head_->next; delete old; } size_ = 0; }
    bool isEmpty() const noexcept { return size_ == 0; }
    std::size_t size() const noexcept { return size_; }
    bool insertBeginning(int value) { Node* n = new Node(value); n->next = head_; head_ = n; ++size_; return true; }
    bool insertEnd(int value) { return insertAt(size_, value); }
    bool insertAt(std::size_t index, int value) {
        if (index > size_) return false;
        if (index == 0) return insertBeginning(value);
        Node* prev = nodeAt(index - 1); Node* n = new Node(value); n->next = prev->next; prev->next = n; ++size_; return true;
    }
    Maybe<int> deleteBeginning() { if (!head_) return {}; Node* n = head_; int v = n->value; head_ = n->next; delete n; --size_; return v; }
    Maybe<int> deleteEnd() { return size_ ? deleteAt(size_ - 1) : Maybe<int>{}; }
    Maybe<int> deleteAt(std::size_t index) {
        if (index >= size_) return {};
        if (!index) return deleteBeginning();
        Node* prev = nodeAt(index - 1); Node* n = prev->next; int v = n->value; prev->next = n->next; delete n; --size_; return v;
    }
    Maybe<std::size_t> search(int value) const { std::size_t i = 0; for (Node* n = head_; n; n = n->next, ++i) if (n->value == value) return i; return {}; }
    bool update(std::size_t index, int value) { Node* n = index < size_ ? nodeAt(index) : nullptr; if (!n) return false; n->value = value; return true; }
    void reverse() noexcept { Node* prev = nullptr; Node* cur = head_; while (cur) { Node* next = cur->next; cur->next = prev; prev = cur; cur = next; } head_ = prev; }
    void display(std::ostream& out = std::cout) const { out << '['; for (Node* n = head_; n; n = n->next) { if (n != head_) out << " -> "; out << n->value; } out << "]\n"; }
};

}  // namespace dsa
