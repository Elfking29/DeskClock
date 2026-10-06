#include <pico/time.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include <array>
#include <string>

#include "comms.hpp"
#include "ds1306.hpp"
#include "tm1637.hpp"
#include "pcf8574.hpp"

#include "screens.hpp"
#include "digital.hpp"
#include "analog.hpp"
#include "life.hpp"
#include "stats.hpp"

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

    // Screens
    Digital digital(rtc, lcd);
    Analog analog(rtc, lcd);
    Life life(rtc, lcd);
    Stats stats(rtc, lcd);
    //Music music(rtc, lcd);
    //Weather weather(rtc, lcd);

    std::array<Screen*, 4> screens = {
        &digital,
        &analog,
        &life,
        &stats
        //&music,
        //&weather
    };
    
    Screen* screen = nullptr;
    Screen* oldscreen = nullptr;

    // Variables
    std::array<uint,7> time;
    std::string line;
    uint64_t update=61;
    uint64_t updisp=0; 
    uint64_t watchdog=0;
    uint64_t poke=0;
    uint64_t elapsed;
    
    while (true){
        sleep_ms(10);
        elapsed=time_us_64()/1000;

        // year, mon, mday, hour, min, sec, wday
        // Display Updates
        time=rtc.get_time();
        if (updisp+5000<elapsed||screen==nullptr){
            if ((time[3]>6&&time[3]<23)||usb.connected()){lcd.backlight(true);}
            else{lcd.backlight(false);}
            screen=screens[get_rand_32()%screens.size()];
            if (screen->check()){
                if (oldscreen!=screen){
                    oldscreen=screen;
                    lcd.clear();
                    screen->setup();
                }
                update=61;
                updisp=elapsed;
            }
            else{screen=nullptr;}
        }
        if (update!=time[5]){
            update=time[5];
            seg.print(padtwo(time[3])+padtwo(time[4]),1-time[5]%2);
            if (screen!=nullptr){screen->display();}
        }

        // Connection Management
        if (usb.connected()){
            if (poke+2000<elapsed){
                poke=elapsed;
                usb.send("ALIVE");
            }

            if (watchdog+5000<elapsed){
                for (Screen* s:screens){s->disconnect();}
                usb.disconnect();
                screen=nullptr;
            }

            line=usb.readline();
            if (line=="ALIVE"){watchdog=elapsed;usb.send("PET");}
            else if (line.substr(0,11)=="///STATS///"){screens[3]->update(line);}
            else if (line!=""){
                //usb.send(line);
            }
        }
        else{
            usb.connect();
            time=usb.rtrars();
            if (time!=std::array<uint,7>{0,0,0,0,0,0,0}){rtc.set_time(time[0], time[1], time[2], time[3], time[4], time[5], time[6]);}
            if (usb.connected()){
                update=61;
                screen=nullptr;
                watchdog=elapsed;
                poke=0;
            }
        }
    }
    return 0;
}