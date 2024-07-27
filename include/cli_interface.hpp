#pragma once
#include <Arduino.h>
#include <iostream>
#include <thread>
#include <esp_now.h>
#include <string>
#include <map>
#include <memory>
#include "psram_custom_allocator.hpp"
#include "commands.hpp"

class CLI {
    private:
    std::thread cliThread;
    public:
    std::map <ps::string, std::unique_ptr <Command>> commandMap;
    CLI();
    ps::map <ps::string, ps::string> parseListToMap(const ps::list <ps::string> &args);
    bool menuEntry();
    void beginAsync();
};


extern CLI cli;