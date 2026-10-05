#include "tm1637.hpp"

#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <string>

TM1637::TM1637(uint ck_pin, uint io_pin,uint bright, bool rotation):ck(ck_pin),io(io_pin),rot(rotation){
    this->bright=bright>7?7:bright;

    gpio_init(io);
    gpio_init(ck);

    gpio_set_dir(io,1);
    gpio_set_dir(ck,1);

    gpio_put(io,0);
    gpio_put(ck,0);

    delay();
    write(0x40);
    write(0x80|0x08|this->bright);
}

void TM1637::delay(){sleep_us(10);}

void TM1637::write_byte(int val){
    for (int i=0; i<8; i++){
        gpio_put(io, (val>>i)&1);
        delay();
        gpio_put(ck,1);
        delay();
        gpio_put(ck,0);
    }
    gpio_put(ck, 0);
    delay();
    gpio_put(ck, 1);
    delay();
    gpio_put(ck, 0);
    delay();
}

void TM1637::write(int val){
    start();
    write_byte(val);
    stop();
}

void TM1637::start(){
    gpio_put(io, 0);
    delay();
    gpio_put(ck,0);
    delay();
}

void TM1637::stop(){
    gpio_put(io, 0);
    delay();
    gpio_put(ck,1);
    delay();
    gpio_put(io,1);
    delay();
}

void TM1637::set_brightness(uint bright){
    this->bright=bright>7?7:bright;
    write(0x40);
    write(0x80|0x08|this->bright);
}

void TM1637::segments(uint a, uint b, uint c, uint d, bool dot){
    write(0x40);
    start();
    write_byte(0xC0);
    write_byte(a);
    write_byte(b|dot*0x80);
    write_byte(c);
    write_byte(d);
    stop();
    start();
    write(0x80|0x08|this->bright);;
    stop();
}

int TM1637::lut(char c){
    switch (c){
        case '0': return 0b00111111;
        case '1': return 0b00000110;
        case '2': return 0b01011011;
        case '3': return 0b01001111;
        case '4': return 0b01100110;
        case '5': return 0b01101101;
        case '6': return 0b01111101;
        case '7': return 0b00000111;
        case '8': return 0b01111111;
        case '9': return 0b01101111;
        case 'A': return 0b01110111;
        case 'B': return 0b01111100;
        case 'C': return 0b00111001;
        case 'D': return 0b01011110;
        case 'E': return 0b01111001;
        case 'F': return 0b01110001;
        default: return 0;
    }
}
void TM1637::print(std::string str, bool dot){
    str.resize(4,' ');
    for (auto &c:str){c=std::toupper(c);}
    if (rot){
        for (size_t i = 0, j = str.length() - 1; i < j; ++i, --j) {
            std::swap(str[i], str[j]);
        }
    }
    uint a=lut(str[0]);
    uint b=lut(str[1]);
    uint c=lut(str[2]);
    uint d=lut(str[3]);
    segments(a,b,c,d,dot);
}