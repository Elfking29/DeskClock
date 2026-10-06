#include "digital.hpp"

#include <array>
#include <string>

Digital::Digital(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}

bool Digital::check() {return true;}

void Digital::setup() {
    lcd.create_char(0, {7,15,31,31,31,31,31,31}); // LT
    lcd.create_char(1, {31,31,31,31,0,0,0,0}); // UB
    lcd.create_char(2, {28,30,31,31,31,31,31,31}); // RT
    lcd.create_char(3, {31,31,31,31,31,31,15,7}); // LL
    lcd.create_char(4, {0,0,0,0,31,31,31,31}); // LB
    lcd.create_char(5, {31,31,31,31,31,31,30,28}); // LR

    lcd.create_char(6,{0,0,3,3,3,3,0,0}); // Left Dot
    lcd.create_char(7,{0,0,24,24,24,24,0,0}); // Right Dot
}

void Digital::disconnect() {}

void Digital::update(std::string key){}

void Digital::display() {
    std::array <uint,7> t = rtc.get_time();
    if (ot!=t[5]){
        ot=t[5];
        print_num(0,t[3]/10);
        print_num(1,t[3]%10);
        print_num(2,t[4]/10);
        print_num(3,t[4]%10);
        print_dots(1-t[5]%2);
    }
}

void Digital::print_num(uint pos, uint num){
    pos%=4;pos=pos==0?0:pos==1?5:pos==2?11:16;
    std::string numstr=num_lut(num);
    std::string sndstr="";
    lcd.pos(0,pos);
    for (size_t i=0; i<numstr.length(); i++){
        if (numstr[i]=='_'){
            lcd.lprint(sndstr);
            sndstr="";
            lcd.pos(i/5+1,pos);
        }
        else if (numstr[i]=='8'){sndstr+='\xFF';}
        else if (numstr[i]=='9'){sndstr+=' ';}
        else {sndstr+='@';sndstr+=numstr[i];}
    }
    lcd.lprint(sndstr);
}

void Digital::print_dots(bool tog){
    lcd.pos(0,9);
    std::string posstr=tog?"99_67_67_99":"99_99_99_99";
    std::string sndstr="";
    for (size_t i=0; i<posstr.length(); i++){
        if (posstr[i]=='_'){
            lcd.lprint(sndstr);
            sndstr="";
            lcd.pos(i/3+1,9);
        }
        else if (posstr[i]=='8'){sndstr+='\xFF';}
        else if (posstr[i]=='9'){sndstr+=' ';}
        else {sndstr+='@';sndstr+=posstr[i];}
    }
    lcd.lprint(sndstr);
}

std::string Digital::num_lut(uint num){
    static const std::array<std::string, 10> lut = {
        "0112_8998_8998_3445",
        "4299_9899_9899_4849",
        "0112_9941_4199_3444",
        "0112_4448_9998_3445",
        "0992_3448_9998_9995",
        "0111_3449_9992_3445",
        "0112_8999_8112_3445",
        "0112_9998_9998_9995",
        "0112_8448_8998_3445",
        "0112_3448_9998_3445"
    };

    return num < 10 ? lut[num] : "9999_9999_9999_9999";
}