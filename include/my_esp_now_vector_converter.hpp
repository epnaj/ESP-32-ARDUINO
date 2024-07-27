#pragma once

#include <convertable_class.hpp>
#include <vector>
#include <type_traits>
#include <esp_types.h>
#include <algorithm>
#include "psram_custom_allocator.hpp"

class EspNowVectorConverter {
    public:
    // this is to convert from integer like to uint8_t
    template <typename T, class Allocator = std::allocator <T>>
    inline static const std::vector <uint8_t, Allocator> toEspNowReadable(const std::vector <T, Allocator> &vec){
        if(std::is_integral <T>::value){
            std::vector <uint8_t, Allocator> vOutput(vec.size());
            std::transform(
                vec.begin(),
                vec.end(),
                vOutput.begin(),
                [](const T &value) -> uint8_t {
                    return static_cast <uint8_t> (value);
                }
            );
            return vOutput;
        }
        return {};
    }
    // this is to convert from uint8_t to integer like
    template <typename T, class Allocator = std::allocator <T>>
    inline static const std::vector <T, Allocator> &toTCharLike(const std::vector <uint8_t, Allocator> &vec){
        std::vector <T, Allocator> vOutput(vec.size());
        std::transform(
            vec.begin(),
            vec.end(),
            vOutput.begin(),
            [](const T &value) -> const T & {
                return static_cast <T> (value);
            }
        );
        return vOutput;
    }

    // this is to convert (deserialize) from byte (uint8_t) to any <EspConvertableClass> 
    template <typename T, class Allocator = std::allocator <T>>
    inline static std::vector <T, Allocator> toEspConvertableClass(const std::vector <uint8_t, Allocator> &vec){
        static_assert(std::is_base_of <EspConvertableClass, T>::value, "T must be subclass of EspConvertableClass!");
        auto sizeOfT = sizeof(T);
        std::vector <T, Allocator> vOutput(vec.size() / sizeOfT);
        // auto vOutputIter = std::begin(vOutput);
        // std::vector <T> vOutput;
        for(auto i = 0; i < vOutput.size(); ++i){
            std::copy(
                std::next(std::begin(vec), i * sizeOfT), 
                std::next(std::begin(vec), (i + 1) * sizeOfT), 
                reinterpret_cast <uint8_t *> (&vOutput[i])
                // vOutput
            );
            // vOutputIter = std::next(vOutputIter);
        }
        return vOutput;
    }

    // this is to convert from <EspConvertableClass> to byte (uint8_t) array
    template <typename T, class Allocator = std::allocator <T>>
    inline static std::vector <uint8_t, Allocator> classToByteArray(const std::vector <T, Allocator> &vec){
        static_assert(std::is_base_of <EspConvertableClass, T>::value, "T must be subclass of EspConvertableClass!");
        auto sizeOfT = sizeof(T);
        // auto outputOffset = 0;
        std::vector <uint8_t, Allocator> vOutput(vec.size() * sizeOfT);
        auto outputIter = std::begin(vOutput);
        for(auto i = 0; i < vec.size(); ++i, outputIter += sizeOfT){
            auto bytePointer = reinterpret_cast <const uint8_t *> (&vec[i]);
            // std::copy(bytePointer, bytePointer + sizeOfT, &(*std::next(std::begin(vOutput), outputOffset)));
            std::copy(bytePointer, bytePointer + sizeOfT, &(*outputIter));
        }
        return vOutput;
    }
};

// template <class C>
// std::vector <C> dataConverter(const uint8_t *data, int data_len) {
//     std::vector <uint8_t> v_temp(data_len);
//     std::copy(data, data + data_len, v_temp.begin());
//     return EspNowVectorConverter::toEspConvertableClass <C> (v_temp)
// }

// template <typename T>
// inline static 

/*
TEST

struct SomeTestClass : virtual public EspConvertableClass {
    uint32_t a, b;
    SomeTestClass() {
        a = 21, b = 37;
    }
    inline const uint8_t headerNumber() { return 0; }
};

int main(){
    SomeTestClass stc;
    std::cout << integral_to_binary_string(stc.a) << " " << integral_to_binary_string(stc.b) << std::endl;
    auto v = EspNowVectorConverter::classToByteArray <SomeTestClass> ({stc});
    for(auto k : v){
        std::cout << integral_to_binary_string(k) << " ";
    }
    std::cout << std::endl;
    // std::cout << integral_to_binary_string() << " " << integral_to_binary_string(stc[0].b) << std::cout;
    auto stc_2 = EspNowVectorConverter::toEspConvertableClass <SomeTestClass> (v);
    std::cout << integral_to_binary_string(stc_2[0].a) << " " << integral_to_binary_string(stc_2[0].b) << std::endl;
    std::cout << stc_2[0].a << stc_2[0].b;
    return 0;
}include/my_esp_now_vector_converter.hpp
*/