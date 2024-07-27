#pragma once
#include <Arduino.h>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include "string_split.hpp"
#include "psram_custom_allocator.hpp"

class MyMAC {
    private:
    void constructorConstString(const ps::string &macAddress) {
        auto macSplit = split(ps::string(macAddress), ":");
        if(macSplit.size() < ESP_NOW_ETH_ALEN){
            std::cout << "Could not parse: " << macAddress << " as MAC\n";
            std::cout << "MAC is NOT set!\n";
            this->_macAddress = {};
            return;
        }
        std::transform(
            std::begin(macSplit),
            std::end(macSplit),
            std::begin(_macAddress),
            [](const ps::string &byte) -> uint8_t {
                auto hexByte{filterNonHex(byte)};
                return static_cast <uint8_t> (
                    std::strtol(
                        hexByte.size() > 2 ? hexByte.substr(0, 2).c_str() : hexByte.c_str(), 
                        nullptr,
                        16
                    )
                );
            }
        );
        // std::cout << "MAC IS: ";
        // std::copy(_macAddress.begin(), std::prev(_macAddress.end()), std::ostream_iterator <int> (std::cout << std::hex, ":"));
        // if(!_macAddress.empty()){
        //     std::cout << std::hex << static_cast <int> (_macAddress.back()) << std::endl;
        // }
    }
    ps::vector <uint8_t> _macAddress;
    public:
    // MyMAC(const uint8_t *macAddress): _macAddress(sizeof(macAddress) / sizeof(uint8_t), 0) {
    //     std::copy(macAddress, macAddress + sizeof(macAddress) / sizeof(uint8_t), this->_macAddress.begin());
    // };
    // MyMAC(const ps::vector <uint8_t> &macAddress): _macAddress(6, 0) {
    //     std::copy(macAddress.begin(), macAddress.end(), this->_macAddress.begin());
    // };
    MyMAC(const ps::string &macAddress): _macAddress(6, 0) {
        constructorConstString(macAddress);
    };
    MyMAC(const char *macAddress): _macAddress(6, 0) {
        constructorConstString(ps::string{macAddress});
    };
    
    template <class Allocator>
    static void printMac(const std::vector <uint8_t, Allocator> &macAddress) {
        std::copy(macAddress.begin(), std::prev(macAddress.end()), std::ostream_iterator <int> (std::cout << std::hex, ":"));
        if(!macAddress.empty()){
            std::cout << std::hex << static_cast <int> (macAddress.back());
        }
    }
    
    static void printMac(const std::array <uint8_t, ESP_NOW_ETH_ALEN> &macAddress) {
        std::copy(macAddress.begin(), std::prev(macAddress.end()), std::ostream_iterator <int> (std::cout << std::hex, ":"));
        std::cout << std::hex << static_cast <int> (macAddress.back());
    }

    static void printMac(const uint8_t *macAddress, const std::size_t &size) {
        std::copy(macAddress, macAddress + size - 1, std::ostream_iterator <int> (std::cout << std::hex, ":"));
        std::cout << std::hex << static_cast <int> (macAddress[size - 1]);
    }

    ps::vector <uint8_t> get() {
        return this->_macAddress;
    } 
};

// peerlist -d
// peeradd -m DC:DA:0C:57:4C:9C -c 0 -i 0
// peerremove -m DC:DA:0C:57:4C:9C