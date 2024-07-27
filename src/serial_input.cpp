#include "serial_input.hpp"
#include <iostream>
#include <thread>


String readSerial(bool blocking){
    Serial.setTimeout(1);
    String data = Serial.readString();
    while(blocking && !data.endsWith("\n")){
        data += Serial.readString();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    data.trim();
    return data;
}

void writeSerial(const String &data) {
    std::cout << data << std::endl;
}

void writeSerial(const ps::string &data) {
    std::cout << data << std::endl;
}

void writeSerial(const char *data) {
    std::cout << data << std::endl;
}