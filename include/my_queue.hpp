#pragma once
#include <vector>
#include <iterator>
#include "psram_custom_allocator.hpp"

template <typename T>
class FixedSizedQueue {
    private:
    const unsigned int _size;
    unsigned int frontPointer;
    unsigned int backPointer;
    std::vector<T> buffer;

    unsigned int addOne(const unsigned int val){
        return (val + 1) % _size;
    }

    public:
    FixedSizedQueue(const unsigned int size): _size(size+1), buffer(_size), frontPointer(0), backPointer(0) {};
    void push(T value){
        unsigned int bpPlus = addOne(backPointer);
        if(bpPlus != frontPointer){
            buffer[backPointer] = value;
            backPointer = bpPlus;
        }
    }

    T pop(){
        if(frontPointer != backPointer){
            frontPointer = addOne(frontPointer);
            return buffer[(frontPointer + _size - 1) % _size];
        }
        return T{};
    }

    bool empty() {
        return frontPointer == backPointer;
    }

    T *begin() {
        return buffer.begin();
    }

    T *end() {
        return buffer.end();
    }

};