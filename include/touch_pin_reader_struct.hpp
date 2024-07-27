#pragma once
#include <vector>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <thread>
#include <value_mutex.hpp>
#include "psram_custom_allocator.hpp"

class TouchPin {
    uint8_t pinNum;
    int32_t treshold;
    bool on;
    public:
    static ValueMutex <bool> changed;
    TouchPin() = default;
    TouchPin(const uint8_t pinNum, const int32_t treshold): pinNum(pinNum), treshold(treshold), on(false) {}

    inline auto set(const bool &value) -> void {
        on = value;
        if(on){
            TouchPin::changed.set(true);
        }
    }
    inline auto isON() -> bool const {
        return on;
    }
    inline auto getPinNum() -> uint8_t const {
        return pinNum;
    }
    inline auto getTreshold() -> int32_t const {
        return treshold;
    }
};

ValueMutex <bool> TouchPin::changed(false);

struct TouchPinReader {
    TouchPin tp;
    std::mutex pinMutex;
    std::thread pinTaskThread;
    TouchPinReader() = default;
    TouchPinReader(TouchPin &touchPin): tp(touchPin) {}
};

struct TouchPinReaderVector {
    std::vector <TouchPinReader> v;
    TouchPinReaderVector(): v(0) {}
    TouchPinReaderVector(const std::vector <touch_pad_t> &vec, const int32_t treshold): v(vec.size()) {
        for(auto i=0; i<vec.size(); ++i){
            v[i].tp = TouchPin(vec[i], treshold);
        }
    }
};