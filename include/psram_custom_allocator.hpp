#pragma once

#include <Arduino.h>
#include <vector>
#include <list>
#include <string>
#include <map>

// taken from: https://github.com/lbernstone/psram_custom_allocator/blob/master/custom_allocator.ino
// another apporoach: https://gist.github.com/sticilface/84440593f2190d89711b07352d276387
// PSRAM BUILD FLAGS
// "-DBOARD_HAS_PSRAM"
// "-mfix-esp32-psram-cache-issue"


template <class T>
struct PSallocator {
    typedef T value_type;
    PSallocator() = default;
    
    template <class U> 
    constexpr PSallocator(const PSallocator<U>&) noexcept {}

    [[nodiscard]] 
    T* allocate(std::size_t n) {
        if(n > std::size_t(-1) / sizeof(T)) 
            throw std::bad_alloc();
        if(auto p = static_cast <T *> (ps_malloc(n * sizeof(T)))) 
            return p;
        throw std::bad_alloc();
    }
    
    void deallocate(T* p, std::size_t) noexcept { 
        std::free(p); 
    }
};

template <class T, class U>
bool operator==(const PSallocator<T>&, const PSallocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const PSallocator<T>&, const PSallocator<U>&) { return false; }

// Contains PSRAM allocatable std:: containers.
namespace ps {
    template <typename T>
    using vector = std::vector <T, PSallocator <T>>;
    template <typename T>
    using list = std::list <T, PSallocator <T>>;
    
    template <typename Key, typename Val>
    using map = std::map <Key, Val, std::less<Key>, PSallocator <std::pair <const Key, Val>>>;

    using string = std::basic_string <char, std::char_traits <char>, PSallocator <char>>;
}

/*
std::vector<int, PSallocator<int> > v;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println(ESP.getFreePsram());
    v.reserve(900000);
    for (uint32_t x=0; x<900000; x++) {
        v.push_back(x);
    }
    Serial.println(ESP.getFreePsram());
    v.clear();
    v.shrink_to_fit();
    Serial.println(ESP.getFreePsram());
}

void loop() {
    vTaskDelete(NULL);
}
*/