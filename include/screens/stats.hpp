#pragma once

#include "screens.hpp"

#include <array>
#include <string>

class Stats : public Screen {
private:
    std::array<int,5> stats={-1,-1,-1,-1,-1};

    std::string bar(uint percent, uint cells, uint slot);
    std::string pad(std::string m, uint l, char c=' ');
    std::string padtwo(int num);
public:
    Stats(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string key) override;
    void display() override;
};