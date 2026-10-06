#include "stats.hpp"

#include <string>
#include <array>
#include <charconv>

Stats::Stats(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}

bool Stats::check(){return stats!=std::array <int,5> {-1,-1,-1,-1,-1};}

void Stats::disconnect(){stats={-1,-1,-1,-1,-1};}

void Stats::setup(){
    lcd.create_char(0,{31, 0, 0, 0, 0, 0, 0,31});
    // Slots 1-4 can be taken by bars
    // Using slot 4 for other purposes though
    lcd.create_char(5,{31,31,31,31,31,31,31,31});
    lcd.create_char(6,{16,16,16,16,16,16,16,16});
    lcd.create_char(7,{ 1, 1, 1, 1, 1, 1, 1, 1});  
}

void Stats::update(std::string key){
    key.erase(0,11);
    std::string delim = "///STATS///";
    std::string sub;
    size_t pos;
    int i=0;
    while ((pos=key.find(delim)!=std::string::npos)){
        if (i>4){disconnect();return;}
        sub=key.substr(0,pos);
        auto result=std::from_chars(sub.data(),sub.data()+sub.size(),stats[i]);
        if (result.ec!=std::errc()||result.ptr!=sub.data()+sub.size())
            {disconnect();return;}
        key.erase(pos+delim.length());
        i++;
    }
    if (i!=5){disconnect();}
}

std::string Stats::bar(uint percent, uint cells, uint slot){
    if (slot<1||slot>4){return std::string(cells,' ');}
    percent=percent>100?100:percent;
    uint pixels=(cells-2)*5;
    uint total=static_cast<uint>(0.5+percent*(pixels/100.0));
    std::string sndstr="@7";
    for (int i=0; i<total/5; i++){sndstr+="@5";}
    if (total%5){
        uint v = total%5?32-(16>>(total%5-1)):0;
        lcd.create_char(slot,{31,v,v,v,v,v,v,31});
        sndstr+="@"+std::to_string(slot);
    }
    while (sndstr.size()/2<cells-1){sndstr+="@0";}
    if (sndstr.size()>(cells-1)*2){sndstr.erase((cells-1)*2);}
    sndstr+="@6";
    return sndstr;
}

std::string Stats::pad(std::string m, uint l, char c){
    if (m.size()>=l){return m.substr(0,l);}
    return m+std::string(l-m.size(),c);
}

std::string Stats::padtwo(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}

// year, mon, mday, hour, min, sec, wday
void Stats::display(){
    std::array <uint,7> t = rtc.get_time();
    lcd.create_char(4,{14,14-4*(t[5]%2),14,0,14,14-4*(t[5]%2),14,0});
    char chg = stats[4]?'+':'-';
    std::string clock=padtwo(t[3])+"@4"+padtwo(t[4])+"@4"+padtwo(t[5]);
    std::string sndstr=pad("BAT: "+pad(std::to_string(stats[3])+"%",5)+std::to_string(chg)+clock,20);
    sndstr+="CPU: "+pad(std::to_string(stats[0])+"%",4)+bar(stats[0],11,1);
    sndstr+="RAM: "+pad(std::to_string(stats[1])+"%",4)+bar(stats[1],11,1);
    sndstr+="DSK: "+pad(std::to_string(stats[2])+"%",4)+bar(stats[2],11,1);
    lcd.home();
    lcd.lprint(sndstr);
}
