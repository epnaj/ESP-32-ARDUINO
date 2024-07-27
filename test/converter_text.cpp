#include <iostream>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <bitset>
#include <climits>
#include "my_esp_now_vector_converter.hpp"
typedef unsigned char uint8_t;

template<class T>
ps::string integral_to_binary_string(T byte){
    std::bitset<sizeof(T) * CHAR_BIT> bs(byte);
    return bs.to_string();
}

class EspConvertableClass {
    public:
    virtual const uint8_t headerNumber() = 0;
};

struct SomeTestClass : virtual public EspConvertableClass {
    uint32_t a, b;
    SomeTestClass() {
        a = 21, b = 37;
    }
    inline const uint8_t headerNumber() { return 0; }
};

struct BlinkInformation : virtual public EspConvertableClass {
    uint32_t a, b, n;
    const uint8_t headerNumber() { return 1; }
    BlinkInformation() = default;
    BlinkInformation(uint32_t blink_length, uint32_t delay_between_blinks, uint32_t n):
        a(blink_length), b(delay_between_blinks), n(n) {}
};

int main(){
    BlinkInformation stc{21, 37, 0};
    std::cout << integral_to_binary_string(stc.a) << " " << integral_to_binary_string(stc.b) << std::endl;
    auto v = EspNowVectorConverter::classToByteArray <BlinkInformation> ({stc});
    for(auto k : v){
        std::cout << integral_to_binary_string(k) << " ";
    }
    std::cout << std::endl;
    // std::cout << integral_to_binary_string() << " " << integral_to_binary_string(stc[0].b) << std::cout;
    auto stc_2 = EspNowVectorConverter::toEspConvertableClass <BlinkInformation> (v);
    std::cout << "byte size: " << v.size() << std::endl;
    std::cout << integral_to_binary_string(stc_2[0].a) << " " << integral_to_binary_string(stc_2[0].b) << std::endl;
    std::cout << stc_2[0].a << stc_2[0].b << std::endl;
    std::cout << stc_2.size() << std::endl;
    return 0;
}