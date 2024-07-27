#pragma once
#include <vector>
#include <mutex>
#include <value_mutex.hpp>
#include "psram_custom_allocator.hpp"

template <typename T>
class AsyncVector {
    private:
    const size_t _size;
    std::vector <ValueMutex <T>> v;
    ValueMutex <bool> guardian;
    public:
    // AsyncVector() = default;
    AsyncVector(const size_t &size): _size(size), v{std::vector <ValueMutex <T>> {_size}} {};
    // can't use usual getter / setter as we loose control over write property
    inline auto set(const size_t &index, const T &value) -> void {
        this->guardian.set(true);
        v[index].set();
    }

    inline auto get(const size_t &index) -> T &const {
        return v[index].get();
    }

    inline auto changed() -> bool const {
        if(this->guardian.get()) {
            this->guardian.set(false);
            return true;
        }
        return false;
    }

    size_t size() const {
        return _size;
    }

};