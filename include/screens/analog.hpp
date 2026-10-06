#pragma once

#include "screens.hpp"

#include <string>
#include <vector>

class Analog : public Screen {
private:
    uint ot=61;

    std::vector<std::pair<uint, uint>> line(int x1, int y1);
    std::vector<std::pair<uint,uint>> pixel_lines(uint minute, uint hour);
    std::vector<std::array<uint, 3>> pix_set(const std::vector<std::pair<uint, uint>>& use);
    void lcd_print(std::vector<std::array<uint, 3>> chars);
    void analog(uint h, uint m);

    std::string center(const std::string& s, const char c='\xFF', uint n=14);
    std::string ordate(uint n);
    std::string getm(uint n);
    std::string getd(uint n);
    std::string padtwo(int num);

public:
    Analog(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string key) override;
    void display() override;
};