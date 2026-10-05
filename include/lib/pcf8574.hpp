#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"
# include <string>
# include <array>

class PCF8574{
    private:
        i2c_inst_t * const bus;
        const uint scl;
        const uint sda;
        const uint addr;
        bool bright;
        const uint rows;
        const uint cols;

        uint row;
        uint col;

        void send(uint value, bool data=false);
        void write_nibble(uint val);
        void write(uint val);

        void send_char(uint c);

    public:
        PCF8574(uint scl, uint sda, uint addr=0x27, bool backlight=true, uint rows=4, uint cols=20);

        void clear();
        void home();
        void backlight(bool enable);
        void enabled(bool enable);
        void pos(uint r, uint c);
        void print(std::string s);
        void create_char(uint loc, std::array<uint,8> data);
        void cprint(uint loc);
        void lprint(std::string str);

};