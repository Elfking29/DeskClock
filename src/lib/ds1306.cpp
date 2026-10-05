#include "ds1306.hpp"

#include "pico/stdlib.h"
#include <hardware/gpio.h>
#include <array>

DS1306::DS1306(uint io_pin, uint ck_pin, uint ce_pin):io(io_pin),ck(ck_pin),ce(ce_pin){
    gpio_init(io);
    gpio_init(ck);
    gpio_init(ce);

    gpio_set_dir(io,1);
    gpio_set_dir(ck,1);
    gpio_set_dir(ce,1);

    gpio_put(io,0);
    gpio_put(ck,0);
    gpio_put(ce,0);
}

int DS1306::clamp(int num, int a, int b){
    if (num>a&&num>b){return a>b?a:b;}
    else if (num<a&&num<b){return a<b?a:b;}
    return num;
}
uint DS1306::bcdtodec(uint num){return ((num>>4)*10)+(num&0x0F);}
uint DS1306::dectobcd(uint num){return ((num/10)<<4|(num%10));}

void DS1306::setwp(bool wp){
    gpio_put(ce,0);
    write(0x8F, wp?read_reg(0x0F)|0x40:read_reg(0x0F)&~0x40);
    gpio_put(ce,0);
}

int DS1306::read(int addr){
    if (addr!=-1){
        gpio_set_dir(io,1);
        gpio_put(ce,1);
        for (int i=0; i<8; i++){
            gpio_put(io,addr&1);
            gpio_put(ck,1);
            sleep_us(1);
            gpio_put(ck,0);
            sleep_us(1);
            addr>>=1;
        }
    }
    uint data=0;
    gpio_set_dir(io,0);
    for (int i=0; i<8; i++){
        data|=gpio_get(io)<<i;
        gpio_put(ck,1);
        sleep_us(1);
        gpio_put(ck,0);
        sleep_us(1);
    }
    return data;
}

void DS1306::write(int addr, int val){
    if (addr!=-1){
        gpio_set_dir(io,1);
        sleep_us(5);
        gpio_put(ce,1);
        sleep_us(5);
        for (int i=0; i<8; i++){
            gpio_put(io,addr&1);
            sleep_us(5);
            gpio_put(ck,1);
            sleep_us(5);
            gpio_put(ck,0);
            sleep_us(5);
            addr>>=1;
        }
    }
    for (int i=0; i<8; i++){
        gpio_put(io,val&1);
        sleep_us(5);
        gpio_put(ck,1);
        sleep_us(5);
        gpio_put(ck,0);
        sleep_us(5);
        val>>=1;
    }
}

int DS1306::read_reg(int addr){
    int data=read(addr);
    gpio_put(ce,0);
    return data;
}

void DS1306::write_reg(int addr, int val){
    setwp(0);
    write(addr,val);
    setwp(1);
}

void DS1306::set_time(uint year, uint mon, uint mday, uint hour, uint min, uint sec, uint wday){
    std::array<uint, 7> data;
    data[0] = dectobcd(clamp(sec, 0, 59));
    data[1] = dectobcd(clamp(min, 0, 59));
    if (th) {
        uint thc = clamp(hour, 0, 23) % 12;
        if (thc == 0){thc = 12;}
        data[2] = dectobcd(thc) | 0x40;
        if (clamp(hour, 0, 23) < 12){data[2] &= ~0x20;}
        else{data[2] |= 0x20;}
    }
    else {data[2] = dectobcd(clamp(hour, 0, 23)) & ~0x40;}
    data[3] = dectobcd(clamp(wday+1,0,6));
    data[4] = dectobcd(clamp(mday, 1, 31));
    data[5] = dectobcd(clamp(mon, 1, 12));
    data[6] = dectobcd(clamp(year % 100, 0, 99));

    setwp(0);
    write(0x80, data[0]);
    for (int i=0; i<6; i++){
        write(-1, data[i+1]);
    }
    setwp(1);
}

std::array<uint, 7> DS1306::get_time(){
    std::array<uint, 7> data;
    data[0] = read(0x00);
    for (int i = 0; i < 6; i++)
        data[i + 1] = read(-1);
    gpio_put(ce, 0);
    uint sec  = bcdtodec(data[0] & 0x7F);
    uint min  = bcdtodec(data[1] & 0x7F);
    uint hour;
    if (th) {
        uint raw_hour = bcdtodec(data[2] & 0x1F);
        uint pm = (data[2] & 0x20) ? 12 : 0;
        hour = raw_hour % 12 + pm;
    }
    else {hour = bcdtodec(data[2] & 0x3F);}
    uint wday = bcdtodec(data[3]&0x07)-1;
    uint mday = bcdtodec(data[4] & 0x3F);
    uint mon  = bcdtodec(data[5] & 0x1F);
    uint year = bcdtodec(data[6]) + 2000;

    return {year, mon, mday, hour, min, sec, wday};
}
