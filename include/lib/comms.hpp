#pragma once

#include "pico/stdlib.h"
#include <string>
#include <array>

class COMMS{
    private:
        bool con;
        std::string buffer;
    public:
        COMMS();

        bool connected();
        void connect();

        void send(std::string& str);
        std::string readline(); 
};