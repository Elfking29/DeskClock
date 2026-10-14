#include "rss.hpp"

#include <string>
#include <array>
#include <charconv>

Rss::Rss(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}


bool Rss::check(){
    return false;
}

void Rss::setup(){

}

void Rss::disconnect(){

}

void Rss::update(std::string& key){

}

void Rss::display(){

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