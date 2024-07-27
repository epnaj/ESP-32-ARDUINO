#pragma once
#include <Arduino.h>
#include <iostream>
#include <thread>
#include <esp_now.h>
#include "sha256.hpp"
#include "serial_input.hpp"
#include "string_split.hpp"
#include "serial_input.hpp"
#include "my_MAC.hpp"
#include "psram_custom_allocator.hpp"
#include "esp_now_peer_handler.hpp"

class Command {
    protected:
    ps::list <ps::string> arguments;
    public:
    virtual bool handle(ps::map <ps::string, ps::string> &parsedArguments) = 0;
    virtual ps::string printHelp() = 0;
};

class Empty : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class Passwd : public Command {
    public:
    static String salt;
    // salted hash of hardcoded passowrd
    // M@5terhi
    static String passwordHash;
    // returns true in case of successfull login
    static bool masterAuthenticate();
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class PeerAdd : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class PeerRemove : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class PeerList : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class Exit : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class Help : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class PMK : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

class Sleep : public Command {
    public:
    bool handle(ps::map <ps::string, ps::string> &parsedArguments);
    ps::string printHelp();
};

// not sure if I should give such abilities to potential user
// class Send : public Command {
//     public:
//     bool handle(ps::map <ps::string, ps::string> &parsedArguments);
//     void printHelp();
// };
