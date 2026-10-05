#pragma once

#include "pico/stdlib.h"
# include <string>

class TM1637{
    private:
        const uint ck;
        const uint io;
        int bright;
        bool rot;

        void write_byte(int val);
        void write(int val);
        void delay();

        void start();
        void stop();

        void segments(uint a, uint b, uint c, uint d, bool dot);
        int lut(char c);

    public:

        TM1637(uint ck_pin, uint io_pin, uint bright=0, bool rot=false);

        void set_brightness(uint bright);
        void print(std::string str, bool dot=false);


};