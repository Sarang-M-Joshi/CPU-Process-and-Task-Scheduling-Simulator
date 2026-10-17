#pragma once

#include "Maybe.h"

#include <cstddef>
#include <iostream>
#include <memory>

namespace dsa {

class StackArray {
    std::unique_ptr<int[]> data_;
    std::size_t capacity_;
    std::size_t count_{0};
public:
    explicit StackArray(std::size_t capacity = 100) : data_(new int[capacity]), capacity_(capacity) {}
    StackArray(const StackArray&) = delete;
    StackArray& operator=(const StackArray&) = delete;
    bool push(int value) { if (isFull()) return false; data_[count_++] = value; return true; }
    Maybe<int> pop() { if (isEmpty()) return {}; return data_[--count_]; }
    Maybe<int> peek() const { return isEmpty() ? Maybe<int>{} : Maybe<int>{data_[count_ - 1]}; }
    void display(std::ostream& out = std::cout) const { out << "Top -> "; for (std::size_t i = count_; i > 0; --i) out << data_[i - 1] << ' '; out << "<- Bottom\n"; }
    bool isEmpty() const noexcept { return count_ == 0; }
    bool isFull() const noexcept { return count_ == capacity_; }
    std::size_t size() const noexcept { return count_; }
};

}  // namespace dsa
