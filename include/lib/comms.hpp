#pragma once

#include "pico/stdlib.h"
#include <string>
#include <array>

class COMMS{
    private:
        bool con;
        std::string buffer;
        std::array<uint,7> time;
    public:
        COMMS();

        bool connected();
        void disconnect();
        void connect();

        void send(const std::string& str);
        void send(const char* str);
        std::string readline();
        std::array<uint,7> rtrars();
};