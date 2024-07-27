#pragma once
#include <mutex>
#include "psram_custom_allocator.hpp"


template <typename T>
class ValueMutex {
    private:
    T value;
    std::mutex mutex;

    public:
    ValueMutex(): value(T{}) {}
    ValueMutex(const T &initialValue): value(initialValue) {}

    inline auto set(const T &value) -> void {
        const std::lock_guard <std::mutex> lock(mutex);
        this->value = value;
    }
    
    inline auto get() -> T const {
        return value;
    }
};