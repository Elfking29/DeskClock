#pragma once

#include "screens.hpp"

#include <array>
#include <string>

class Weather : public Screen {
private:
    std::string location="";
    std::array<int,6> weather={-1,-1,-1,-1,-1,-1};

    std::string longcode(int c);
    std::string smallcode(int c);
    void gcode(int c);

    std::string pad(std::string m, uint l, char c=' ');
    std::string padtwo(int num);
    std::string center(const std::string& s, const char c=' ', uint n=20);

public:
    Weather(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string& key) override;
    void display() override;
};