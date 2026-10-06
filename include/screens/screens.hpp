#pragma once

#include "ds1306.hpp"
#include "pcf8574.hpp"
#include <string>

class Screen {
protected:
    DS1306& rtc;
    PCF8574& lcd;
public:
    Screen (DS1306& rtc, PCF8574& lcd);
    virtual bool check() = 0;
    virtual void setup() = 0;
    virtual void disconnect() = 0;
    virtual void update(std::string key) = 0;
    virtual void display() = 0;
};