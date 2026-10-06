#include <pico/time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include <array>
#include <string>

#include "comms.hpp"
#include "ds1306.hpp"
#include "tm1637.hpp"
#include "pcf8574.hpp"

std::string padtwo(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}

int main(){
    stdio_init_all();

    // Drivers
    COMMS usb;
    DS1306 rtc(13,14,15);
    TM1637 seg(1,0);
    PCF8574 lcd(17,16);

    // Sources



    // Variables
    std::array<uint,7> time;
    std::string line;
    uint64_t update=61;
    uint64_t screen=0; 
    uint64_t watchdog=0;
    uint64_t poke=0;
    uint64_t elapsed;
    
    while (true){
        elapsed=time_us_64()/1000;
        sleep_ms(10);

        // Connection Management
        if (usb.connected()){
            if (poke+2000<elapsed){
                poke=elapsed;
                usb.send("ALIVE");
            }

            if (watchdog+5000<elapsed){
                usb.disconnect();
            }

            line=usb.readline();
            if (line=="ALIVE"){watchdog=elapsed;usb.send("PET");}
            else if (line!=""){usb.send(line);}
        }
        else{
            usb.connect();
            time=usb.rtrars();
            if (time!=std::array<uint,7>{0,0,0,0,0,0,0}){rtc.set_time(time[0], time[1], time[2], time[3], time[4], time[5], time[6]);}
            if (usb.connected()){
                update=61;
                watchdog=elapsed;
                poke=0;
            }
        }

        // Display Updates
        // year, mon, mday, hour, min, sec, wday
        time=rtc.get_time();
        if (update!=time[5]){
            seg.print(padtwo(time[3])+padtwo(time[4]),1-time[5]%2);
        }

    }
    return 0;
}