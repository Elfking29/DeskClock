#include "weather.hpp"

#include <string>
#include <array>
#include <charconv>

Weather::Weather(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}


bool Weather::check(){
    bool t1=location!="";
    bool t2=weather!=std::array<int,6> {-1,-1,-1,-1,-1,-1};
    return t1&&t2;
}

void Weather::setup(){lcd.create_char(6,{14,10,14,0,0,0,0,0});}

void Weather::disconnect(){
    location="";
    weather={-1,-1,-1,-1,-1,-1};
}

void Weather::update(std::string& key){
    if (key.substr(0,11)!="///WTHER///"){return;}
    key.erase(0,11);
    std::string delim = "///SPLIT///";
    std::string sub;
    size_t pos;
    int i=0;
    while ((pos=key.find(delim))!=std::string::npos){
        if (i>7){disconnect();return;}
        sub=key.substr(0,pos);
        if (i==0){
            if (sub==" "){location="";}
            else{location=sub;}
        }
        else{
            auto result=std::from_chars(sub.data(),sub.data()+sub.size(),weather[i-1]);
            if (result.ec!=std::errc()||result.ptr!=sub.data()+sub.size())
                {disconnect();return;}
        }
        key.erase(0,pos+delim.length());
        i++;
    }
    if (i!=7){disconnect();}
}

void Weather::display(){
    std::array <uint,7> t = rtc.get_time();
    lcd.create_char(7,{14,14-4*(t[5]%2),14,0,14,14-4*(t[5]%2),14,0});
    std::string city=center(location.substr(0,location.find("|")).substr(0,10),'\xFF',10);
    std::string sndstr="@0@1@2\xFF"+city+"\xFF"+padtwo(t[3])+"@7"+padtwo(t[4]);
    sndstr+="@3@4@5\xFF"+center(longcode(weather[2]),'\xFF',16);
    std::string tststr="T: "+std::to_string(weather[0])+"@6 H: "+std::to_string(weather[1])+"%";
    if (tststr.size()>15){tststr="T "+std::to_string(weather[0])+"@6 H "+std::to_string(weather[1])+"%";}
    sndstr+=center(tststr,'\xFF',20);
    tststr="1H: "+smallcode(weather[5])+" T: "+std::to_string(weather[3])+"@6 R: "+std::to_string(weather[4])+"%";
    if (tststr.size()>21){tststr="1H: "+smallcode(weather[5])+" T "+std::to_string(weather[3])+"@6 R "+std::to_string(weather[4])+"%";}
    sndstr+=center(tststr,'\xFF',20);
    gcode(weather[2]);
    lcd.lprint(sndstr);
}

std::string Weather::pad(std::string m, uint l, char c){
    if (m.size()>=l){return m.substr(0,l);}
    return m+std::string(l-m.size(),c);
}

std::string Weather::padtwo(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}

std::string Weather::center(const std::string& s,  const char c, uint n){
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

std::string Weather::longcode(int c){
    if (c == 0){return "Clear";}
    else if (c == 1){return "Mostly Clear";}
    else if (c == 2){return "Partly Cloudy";}
    else if (c == 3){return "Cloudy";}
    else if (c == 45 || c == 48){return "Fog";}
    else if (c == 51 || c == 61 || c == 80){return "Light Rain";}
    else if (c == 53 || c == 63 || c == 81){return "Rain";}
    else if (c == 55 || c == 65 || c == 82){return "Heavy Rain";}
    else if (c == 56 || c == 66){return "Light Ice Rain";}
    else if (c == 57 || c == 67){return "Heavy Ice Rain";}
    else if (c == 71 || c == 77 || c == 85){return "Light Snow";}
    else if (c == 73) {return "Snow";}
    else if (c == 75 || c == 86){return "Heavy Snow";}
    else if (c == 95 || c == 97){return "Thunderstorm";}
    else if (c == 96 || c == 99){return "Hail & Thunder";}
    return "";
}

std::string Weather::smallcode(int c){
    if (c == 0){return "CL";}
    else if (c == 1){return "MC";}
    else if (c == 2){return "PC";}
    else if (c == 3){return "CD";}
    else if (c == 45 || c == 48){return "FG";}
    else if (c == 51 || c == 61 || c == 80){return "LR";}
    else if (c == 53 || c == 63 || c == 81){return "RN";}
    else if (c == 55 || c == 65 || c == 82){return "HR";}
    else if (c == 56 || c == 66){return "LI";}
    else if (c == 57 || c == 67){return "HI";}
    else if (c == 71 || c == 77 || c == 85){return "LS";}
    else if (c == 73){return "SN";}
    else if (c == 75 || c == 86){return "HS";}
    else if (c == 95 || c == 97){return "TH";}
    else if (c == 96 || c == 99){return "HH";}
    return "  ";
}

void Weather::gcode(int c){
    uint chars[6][8];

    if (c == 0) {
        uint a[6][8] = {
            {0,0,4,2,1,0,0,14},
            {0,4,4,4,0,14,31,31},
            {0,0,4,8,16,0,0,14},
            {0,0,1,2,4,0,0,0},
            {31,14,0,4,4,4,0,0},
            {0,0,16,8,4,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 1) {
        uint a[6][8] = {
            {0,0,4,2,1,0,0,14},
            {0,4,4,4,0,14,31,31},
            {0,14,31,31,14,0,0,14},
            {0,0,0,14,31,31,14,0},
            {31,14,0,4,4,4,0,0},
            {0,0,16,8,4,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 2) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,0,1,2,4,0,0,0},
            {31,14,0,4,4,4,0,0},
            {0,0,16,8,4,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 3) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {7,3,3,1,0,0,0,0},
            {31,31,31,24,0,0,0,0},
            {28,24,16,0,0,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 45 || c == 48) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,0,9,6,0,9,6,0},
            {0,24,4,3,24,4,3,0},
            {0,12,18,0,12,18,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 51 || c == 61 || c == 80) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,2,4,0,0,0,0,0},
            {0,4,8,0,0,0,0,0},
            {0,8,16,0,0,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 53 || c == 63 || c == 81) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,1,2,4,0,0,0,0},
            {0,2,4,8,0,0,0,0},
            {0,4,8,16,0,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 55 || c == 65 || c == 82) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,1,2,4,1,2,4,0},
            {0,2,4,8,2,4,8,0},
            {0,4,8,16,4,8,16,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 56 || c == 66) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,2,4,0,0,0,0,0},
            {0,2,4,0,0,0,0,0},
            {0,10,21,10,21,10,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 57 || c == 67) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,2,4,8,2,4,8,0},
            {0,2,4,8,2,4,8,0},
            {0,14,21,27,21,14,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 71 || c == 77 || c == 85) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,0,2,7,2,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,8,28,8,0,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 73) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,0,2,7,2,0,0,0},
            {0,0,0,0,4,14,4,0},
            {0,8,28,8,0,0,0,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 75 || c == 86) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {2,8,2,7,2,8,2,0},
            {9,0,2,8,4,14,5,0},
            {2,8,28,8,4,0,10,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else if (c == 95 || c == 96 || c == 97 || c == 99) {
        uint a[6][8] = {
            {0,0,0,0,1,3,7,15},
            {0,0,1,3,31,31,31,31},
            {0,0,16,24,28,28,30,30},
            {0,1,2,4,1,2,4,0},
            {3,6,4,12,8,24,16,0},
            {0,4,8,16,4,8,16,0}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }
    else {
        uint a[6][8] = {
            {1,0,1,0,1,0,1,0},
            {0,1,0,1,0,1,0,1},
            {1,0,1,0,1,0,1,0},
            {0,1,0,1,0,1,0,1},
            {1,0,1,0,1,0,1,0},
            {0,1,0,1,0,1,0,1}
        };
        for (int i=0;i<6;i++)
            for (int j=0;j<8;j++)
                chars[i][j]=a[i][j];
    }

    for (int i=0;i<6;i++)
        lcd.create_char(i, {
            chars[i][0], chars[i][1],
            chars[i][2], chars[i][3],
            chars[i][4], chars[i][5],
            chars[i][6], chars[i][7]
        });
}