#include <pico/time.h>
#include <stdio.h>
#include "pcf8574.hpp"
#include "pico/stdlib.h"
#include <array>
#include <string>

#include "comms.hpp"
#include "ds1306.hpp"
#include "tm1637.hpp"
#include "pcf8574.hpp"

std::string pad2(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}


int main()
{
    stdio_init_all();
    DS1306 rtc(13,14,15);
    TM1637 seg(1,0);
    PCF8574 lcd(17,16);
    rtc.set_time(2026, 10, 5, 11, 38, 0, 0);
    std::array<uint,7> time;
    uint ot=61;
    while (true) {
        time=rtc.get_time();
        if (ot!=time[5]){
            ot=time[5];
            seg.print(pad2(time[3])+pad2(time[4]),1-time[5]%2);
            lcd.create_char(0,{14,14-4*(time[5]%2),14,0,14,14-4*(time[5]%2),14,0});
            lcd.home();
            lcd.lprint(pad2(time[3])+"@0"+pad2(time[4])+"@0"+pad2(time[5]));
        }
        sleep_ms(10);
    }
}
