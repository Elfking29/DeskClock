#pragma once

#include "screens.hpp"

class Digital : public Screen {
private:
    uint ot=61;

    void print_num(uint pos, uint num);
    void print_dots(bool tog);
    std::string num_lut(uint num);
public:
    Digital(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string key) override;
    void display() override;
};