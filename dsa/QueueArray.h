#pragma once

#include "Maybe.h"

#include <cstddef>
#include <iostream>
#include <memory>

namespace dsa {

class QueueArray {
    std::unique_ptr<int[]> data_;
    std::size_t capacity_;
    std::size_t front_{0}, rear_{0}, count_{0};
public:
    explicit QueueArray(std::size_t capacity = 100) : data_(new int[capacity]), capacity_(capacity) {}
    QueueArray(const QueueArray&) = delete;
    QueueArray& operator=(const QueueArray&) = delete;
    bool enqueue(int value) { if (isFull()) return false; data_[rear_] = value; rear_ = (rear_ + 1) % capacity_; ++count_; return true; }
    Maybe<int> dequeue() { if (isEmpty()) return {}; int v = data_[front_]; front_ = (front_ + 1) % capacity_; --count_; return v; }
    Maybe<int> front() const { return isEmpty() ? Maybe<int>{} : Maybe<int>{data_[front_]}; }
    void display(std::ostream& out = std::cout) const { out << "Front -> "; for (std::size_t i = 0; i < count_; ++i) out << data_[(front_ + i) % capacity_] << ' '; out << "<- Rear\n"; }
    bool isEmpty() const noexcept { return count_ == 0; }
    bool isFull() const noexcept { return count_ == capacity_; }
    std::size_t size() const noexcept { return count_; }
};

}  // namespace dsa
