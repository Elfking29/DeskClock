#include "screens.hpp"

#include "ds1306.hpp"
#include "pcf8574.hpp"

Screen::Screen(DS1306& rtc, PCF8574& lcd):rtc(rtc),lcd(lcd){}