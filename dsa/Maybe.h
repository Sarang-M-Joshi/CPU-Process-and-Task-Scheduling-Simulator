#pragma once

namespace dsa {

// Small optional-like result for compatibility with older C++17 standard libraries.
template <typename T>
class Maybe {
    T value_{};
    bool present_{false};
public:
    Maybe() = default;
    Maybe(const T& value) : value_(value), present_(true) {}
    explicit operator bool() const noexcept { return present_; }
    const T& operator*() const noexcept { return value_; }
    bool operator==(const T& value) const noexcept { return present_ && value_ == value; }
};

}  // namespace dsa
