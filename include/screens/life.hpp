#pragma once

#include "screens.hpp"

#include <array>
#include <string>

class Life : public Screen {
private:
    static constexpr uint lw=5;
    static constexpr uint hi=45;
    static constexpr uint rows=8;
    static constexpr uint cols=17;
    uint repeat=0;

    bool board[rows/2*2][cols];
    bool oldboard[rows/2*2][cols];
    uint fixboard[rows/2][cols];

    void randSet(uint percent);
    int checkCell(uint r, uint c);
    void runCycle();
    void boardFix();
    uint randint(uint l, uint h);
    std::string padtwo(uint num);

public:
    Life(DS1306& rtc, PCF8574& lcd);

    bool check() override;
    void setup() override;
    void disconnect() override;
    void update(std::string key) override;
    void display() override;
};