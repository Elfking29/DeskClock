#include "pcf8574.hpp"

#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <hardware/structs/io_bank0.h>
#include <pico/time.h>
#include <string>
#include <array>


PCF8574::PCF8574(uint scl, uint sda, uint addr, bool backlight, uint rows, uint cols):
bus(i2c0),scl(scl),sda(sda),addr(addr),bright(backlight),rows(rows),cols(cols)
{
    i2c_init(bus,100000);
    gpio_set_function(scl,GPIO_FUNC_I2C);
    gpio_set_function(sda,GPIO_FUNC_I2C);
    gpio_pull_up(scl);
    gpio_pull_up(sda);
    sleep_ms(5);

    send(0x03,0);
    sleep_ms(5);
    send(0x03,0);
    sleep_ms(5);
    send(0x03,0);
    sleep_us(100);
    send(0x02,0);
    sleep_ms(5);

    send(0x20|(rows==1?0:8),0);
    sleep_us(50);
    send(0x04|0x02,0);
    sleep_us(50);
    send(0x08|0x04,0);
    sleep_us(50);

    clear();
}

void PCF8574::send(uint val, bool data){
    write_nibble(data|(val&0xF0)|(bright?0x08:0x00));
    write_nibble(data|((val<<4)&0xF0)|(bright?0x08:0x00));
}

void PCF8574::write_nibble(uint val){
    uint8_t v = val;
    v&=~4;
    i2c_write_blocking(bus, addr, &v, 1, false);
    sleep_us(1);
    v|=4;
    i2c_write_blocking(bus, addr, &v, 1, false);
    sleep_us(1);
    v&=~4;
    i2c_write_blocking(bus, addr, &v, 1, false);
    sleep_us(100);
}

void PCF8574::write(uint val){
    uint8_t v = val;
    i2c_write_blocking(bus, addr, &v, 1, false);
}

void PCF8574::clear(){
    send(0x01);
    sleep_ms(2);
    home();
}

void PCF8574::home(){
    send(0x02,0);
    row=0;
    col=0;
    sleep_ms(2);
}

void PCF8574::backlight(bool enable){
    bright=enable;
    write(bright?0x08:0x00);
}

void PCF8574::enabled(bool enable){
    send(0x08|(enable?0x04:0x00),0);
    sleep_us(50);
}

void PCF8574::pos(uint r, uint c){
    row=r%rows;
    col=c%cols;
    send(0x80|(row==0?0:row==1?0x40:row==2?cols:0x40+cols)+col,0);
    sleep_us(50);
}

void PCF8574::print(std::string s){
    for (int i=0; i<s.length(); i++){
        if (s[i]=='\n'){pos((row+1)%rows,0);}
        else{
            send(static_cast<uint8_t>(s[i]),1);
            if (col<cols-1){col+=1;}
            else{
                row=(row+1)%rows;
                col=0;
            }
        }
        pos(row,col);
    }
}

void PCF8574::create_char(uint loc, std::array<uint,8> data){
    loc%=8;
    uint sr=row;
    uint sc=col;
    send(0x40|loc<<3,0);
    for (int i=0; i<8; i++){send(data[i],1);}
    pos(sr,sc);
}

void PCF8574::cprint(uint loc){
    send(loc%8,1);
    if (col<cols - 1){col++;}
    else{
        row=(row+1)%rows;
        col=0;
    }
    pos(row, col);
}

void PCF8574::lprint(std::string s){
    std::string text;

    for (int i = 0; i < s.length(); i++){
        if (s[i] == '@' && i + 1 < s.length() &&
            s[i + 1] >= '0' && s[i + 1] <= '7'){

            if (!text.empty()){
                print(text);
                text.clear();
            }

            cprint(s[i + 1] - '0');
            i++;
        }
        else{
            text += s[i];
        }
    }

    if (!text.empty()){
        print(text);
    }
}