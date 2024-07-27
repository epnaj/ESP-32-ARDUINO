#pragma once
#include <esp_types.h>
#include <vector>
#include <memory>
#include "psram_custom_allocator.hpp"

// ALL SUBCLASSES MUST HAVE DEFAULT PARAMETERLESS CONSTRUCTORS! 
struct EspConvertableClass {
    virtual const uint8_t headerNumber() = 0;
};

struct BlinkInformation : virtual public EspConvertableClass {
    uint32_t blink_length, delay_between_blinks, n;
    const uint8_t headerNumber() { return 1; }
    BlinkInformation() = default;
    BlinkInformation(uint32_t blink_length, uint32_t delay_between_blinks, uint32_t n):
        blink_length(blink_length), delay_between_blinks(delay_between_blinks), n(n) {}
};

struct TestClass : virtual public EspConvertableClass {
    const uint8_t headerNumber() { return 2; }
};

// std::unordered_map <int, > headerTypeMap;
// types = [A, B, C]

// typeTaken = types[0]

// typeTaken instance = someTemplatedFunction <typeTaken> ();