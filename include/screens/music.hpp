#pragma once

#include "screens.hpp"

#include <array>
#include <string>
#include <vector>

class Music : public Screen {
private:
    std::array<std::string,4> taas={" "," "," "," "};
    std::array<std::string,3> otaa={" "," "," "};
    std::array<int,5> pppvm={-1,-1,-1,-1,-1};
    std::vector<std::string> players{" "};

    std::array<uint,3> mtc={0,0,0};

    std::string bar(float percent, uint cells, uint slot, bool mini=false);
    std::string pad(std::string m, uint l, char c=' ');
    std::string padtwo(int num);
    std::string rotate(std::string line, uint& mtc, uint ml=20, uint mj=3, std::string md=" - ");
    std::string center(const std::string& s, const char c=' ', uint n=20);
public:
    Music(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string key) override;
    void display() override;
};