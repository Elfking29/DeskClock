#include "music.hpp"

#include <string>
#include <array>
#include <charconv>

Music::Music(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}

bool Music::check(){
    bool t1=taas!=std::array <std::string,4> {" "," "," "," "};
    bool t2=pppvm!=std::array <int,5> {-1,-1,-1,-1,-1};
    bool t3=players!=std::vector <std::string> {" "};
    return t1&&t2&&t3;
}

void Music::setup(){
    lcd.create_char(0,{31, 0, 0, 0, 0, 0, 0,31});
    // Slots 1-4 can be taken by bars
    // Using slot 4 for other purposes though
    lcd.create_char(5,{31,31,31,31,31,31,31,31});
    lcd.create_char(6,{16,16,16,16,16,16,16,16});
    lcd.create_char(7,{ 1, 1, 1, 1, 1, 1, 1, 1});  
}

void Music::disconnect(){
    taas={" "," "," "," "};
    otaa={" "," "," "};
    pppvm={-1,-1,-1,-1,-1};
    players.assign(1," ");

    mtc={0,0,0};
}

void Music::update(std::string key){
    key.erase(0,11);
    std::string delim = "///SPLIT///";
    std::string sub;
    size_t pos;
    int i=0;
    while ((pos=key.find(delim))!=std::string::npos){
        if (i>9){disconnect();return;}
        sub=key.substr(0,pos);
        if (i<4){
            taas[i]=sub;
        }
        else if (i!=6){
            auto result=std::from_chars(sub.data(),sub.data()+sub.size(),pppvm[i-4-(i>6)]);
            if (result.ec!=std::errc()||result.ptr!=sub.data()+sub.size())
                {disconnect();return;}
        }
        else{
            size_t jos;
            std::string jelim = "///PSPLT///";
            players.clear();
            while ((jos=sub.find(jelim))!=std::string::npos){
                players.push_back(sub.substr(0,jos));
                sub.erase(0,jos+jelim.length());
            }
        }
        key.erase(0,pos+delim.length());
        i++;
    }
    if (i!=10){disconnect();}

    for (int i=0; i<3; i++){
        if (taas[i]!=otaa[i]){mtc={0,0,0};}
        otaa[i]=taas[i];
    }
}

void Music::display(){
    std::array <uint,7> t = rtc.get_time();
    lcd.create_char(4,{14,14-4*(t[5]%2),14,0,14,14-4*(t[5]%2),14,0});
    lcd.home();
    for (int i=0; i<3; i++){lcd.lprint(rotate(taas[i],mtc[i]));}
    std::string clock=padtwo(t[3])+"@4"+padtwo(t[4]);
    if (taas[3]=="Playing"){lcd.create_char(2,{0,10,10,10,10,10,10,0});}
    else {lcd.create_char(2,{0,8,12,14,14,12,8,0});}
    lcd.lprint("@2"+bar((10000*pppvm[0])/(97.0*pppvm[1]),14,1,false)+clock);
}

std::string Music::bar(float percent, uint cells, uint slot, bool mini){
    if (slot<1||slot>4){return std::string(cells,' ');}
    percent=percent>100?100:percent;
    uint pixels=(cells-2)*5;
    uint total=static_cast<uint>(0.5+percent*(pixels/100.0));
    std::string sndstr="@7";
    for (int i=0; i<total/5; i++){sndstr+="@5";}
    if (total%(mini?30:5)){
        if (!mini){
            uint v = total%5?32-(16>>(total%5-1)):0;
            lcd.create_char(slot,{31,v,v,v,v,v,v,31});
        }
        else{
            uint v[6]={0};
            for (int i=0; i<total%30; i++){v[5-(i%6)]|=16>>i/6;}
            lcd.create_char(slot,{31,v[0],v[1],v[2],v[3],v[4],v[5],31});
        }
        sndstr+="@"+std::to_string(slot);
    }
    while (sndstr.size()/2<cells-1){sndstr+="@0";}
    if (sndstr.size()>(cells-1)*2){sndstr.erase((cells-1)*2);}
    sndstr+="@6";
    return sndstr;
}

std::string Music::pad(std::string m, uint l, char c){
    if (m.size()>=l){return m.substr(0,l);}
    return m+std::string(l-m.size(),c);
}

std::string Music::padtwo(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}

std::string Music::rotate(std::string line, uint& mtc, uint ml, uint mj, std::string md){
    if (line.size()<=ml){return center(line,' ',ml);}
    std::string restr;
    mtc=(mtc+mj)%(line+md).size();
    restr=(line+md+line.substr(0,ml)).substr(mtc,ml);
    return center(restr,' ',ml);
}

std::string Music::center(const std::string& s,  const char c, uint n){
    uint len = 0;
    for (uint i = 0; i < s.size(); i++) {
        if (s[i] == '@' && i + 1 < s.size()) {
            i++;
        }
        len++;
    }
    if (len >= n){return s;}
    uint left = (n - len) / 2;
    uint right = n - len - left;
    return std::string(left, c) + s + std::string(right, c);
}

