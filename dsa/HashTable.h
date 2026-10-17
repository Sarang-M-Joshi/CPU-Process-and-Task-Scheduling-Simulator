#pragma once

#include <cstddef>
#include <ostream>
#include <vector>

namespace dsa {

class HashTable {
    enum class State { Empty, Occupied, Deleted };
    struct Slot { int key{0}; State state{State::Empty}; };
    std::vector<Slot> slots_;
    std::size_t count_{0};
    std::size_t start(int key) const {
        const long long n = static_cast<long long>(slots_.size());
        long long r = static_cast<long long>(key) % n;
        return static_cast<std::size_t>(r < 0 ? r + n : r);
    }
public:
    explicit HashTable(std::size_t capacity = 11) : slots_(capacity) {}
    std::size_t capacity() const { return slots_.size(); }
    std::size_t size() const { return count_; }
    bool insert(int key) {
        if (slots_.empty()) return false;
        std::size_t firstDeleted = slots_.size(), at = start(key);
        for (std::size_t n = 0; n < slots_.size(); ++n, at = (at + 1) % slots_.size()) {
            Slot& slot = slots_[at];
            if (slot.state == State::Occupied && slot.key == key) return false;
            if (slot.state == State::Deleted && firstDeleted == slots_.size()) firstDeleted = at;
            if (slot.state == State::Empty) {
                const std::size_t target = firstDeleted == slots_.size() ? at : firstDeleted;
                slots_[target].key = key; slots_[target].state = State::Occupied; ++count_; return true;
            }
        }
        if (firstDeleted != slots_.size()) { slots_[firstDeleted].key = key; slots_[firstDeleted].state = State::Occupied; ++count_; return true; }
        return false;
    }
    bool contains(int key) const {
        if (slots_.empty()) return false;
        std::size_t at = start(key);
        for (std::size_t n = 0; n < slots_.size(); ++n, at = (at + 1) % slots_.size()) {
            if (slots_[at].state == State::Empty) return false;
            if (slots_[at].state == State::Occupied && slots_[at].key == key) return true;
        }
        return false;
    }
    bool erase(int key) {
        if (slots_.empty()) return false;
        std::size_t at = start(key);
        for (std::size_t n = 0; n < slots_.size(); ++n, at = (at + 1) % slots_.size()) {
            if (slots_[at].state == State::Empty) return false;
            if (slots_[at].state == State::Occupied && slots_[at].key == key) { slots_[at].state = State::Deleted; --count_; return true; }
        }
        return false;
    }
    void display(std::ostream& out) const {
        for (std::size_t i = 0; i < slots_.size(); ++i) {
            out << i << ": ";
            if (slots_[i].state == State::Occupied) out << slots_[i].key;
            else if (slots_[i].state == State::Deleted) out << "<deleted>";
            else out << "<empty>";
            out << '\n';
        }
    }
};
}  // namespace dsa
