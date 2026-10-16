#include "rss.hpp"

#include <string>
#include <array>
#include <charconv>
#include "pico/rand.h"


Rss::Rss(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}

bool Rss::check(){
    if (authors.empty()||articles.empty()||unread==-1){return false;}
    return true;
}

void Rss::setup(){
    lcd.create_char(0,{0,0,0,0,0,0,21,0});
    rand=get_rand_32()%articles.size();
}

void Rss::disconnect(){
    authors.clear();
    articles.clear();
    unread=-1;
    rand=-1;
}

void Rss::display(){
    std::array <uint,7> t = rtc.get_time();
    lcd.create_char(1,{14,14-4*(t[5]%2),14,0,14,14-4*(t[5]%2),14,0});
    std::string first=center(padtwo(t[3])+"@1"+padtwo(t[4])+"@1"+padtwo(t[5])+" Unread: "+std::to_string(unread),' ',20);
    std::string second=center(authors[rand].size()<=20?authors[rand]:authors[rand].substr(0,19)+"@0",' ',20);
    std::string tststr=articles[rand];
    std::string third=center(tststr.substr(0,20),' ',20);
    tststr.erase(0,20);
    std::string fourth=center(tststr.size()<=20?tststr:tststr.substr(0,19)+"@0",' ',20);
    lcd.pos(0,0);
    lcd.lprint(center(aposfix(first),' ',20));
    lcd.pos(1,0);
    lcd.lprint(center(aposfix(second),' ',20));
    lcd.pos(2,0);
    lcd.lprint(center(aposfix(third),' ',20));
    lcd.pos(3,0);
    lcd.lprint(center(aposfix(fourth),' ',20));
}

void Rss::update(std::string& key){
    if (key.substr(0,11)!="///RSSFD///"){return;}
    key.erase(0,11);
    std::string delim = "///SPLIT///";
    std::string sub;
    size_t pos;
    int i=0;
    authors.clear();
    articles.clear();
    unread=-1;
    while ((pos=key.find(delim))!=std::string::npos){
        sub=key.substr(0,pos);
        if (i==0){
            auto result=std::from_chars(sub.data(),sub.data()+sub.size(),unread);
            if (result.ec!=std::errc()||result.ptr!=sub.data()+sub.size())
                {disconnect();return;}
        }
        else if (i%2){
            authors.push_back(sub);
        }
        else {
            articles.push_back(sub);
        }
        key.erase(0,pos+delim.length());
        i++;
    }
    if ((authors.size()!=articles.size())){disconnect();}
}

std::string Rss::aposfix(std::string s){
    for (size_t pos = 0; (pos = s.find("’", pos)) != std::string::npos; pos++) {
        s.replace(pos, 3, "'");
    }
    for (size_t pos = 0; (pos = s.find("‘", pos)) != std::string::npos; pos++) {
        s.replace(pos, 3, "'");
    }
    for (size_t pos = 0; (pos = s.find("“", pos)) != std::string::npos; pos++) {
        s.replace(pos, 3, "\"");
    }
    for (size_t pos = 0; (pos = s.find("”", pos)) != std::string::npos; pos++) {
        s.replace(pos, 3, "\"");
    }
    for (size_t pos = 0; (pos = s.find("–", pos)) != std::string::npos; pos++) {
        s.replace(pos, 3, "-");
    }
    return s;
}

std::string Rss::pad(std::string m, uint l, char c){
    if (m.size()>=l){return m.substr(0,l);}
    return m+std::string(l-m.size(),c);
}

std::string Rss::padtwo(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}

std::string Rss::center(const std::string& s,  const char c, uint n){
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