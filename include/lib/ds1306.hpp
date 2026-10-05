#pragma once

#include "pico/stdlib.h"
# include <array>

class DS1306{
    private:
        const uint ce;
        const uint io;
        const uint ck;

        bool th=0;

        void write(int addr, int val);
        int read(int addr);

        void write_reg(int addr, int val);
        int read_reg(int addr);

        void setwp(bool wp);

    public:

        static uint bcdtodec(uint num);
        static uint dectobcd(uint num);
        static int clamp(int num, int a, int b);
        static void print_time(std::array<uint,7> time);

        DS1306(uint io_pin, uint ck_pin, uint ce_pin);

        void set_time(uint year, uint mon, uint mday, uint hour, uint min, uint sec, uint wday);
        std::array<uint,7> get_time();

};