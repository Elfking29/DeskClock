#include "analog.hpp"

#include <array>
#include <vector>
#include <string>
#include <cmath>

Analog::Analog(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}

bool Analog::check() {return true;}

void Analog::setup() {}

void Analog::disconnect() {}

void Analog::update(std::string key){}

void Analog::display() {
    std::array <uint,7> t = rtc.get_time();
    if (ot!=t[5]){
        ot=t[5];
        analog(t[3],t[4]);
        lcd.create_char(7,{14,14-4*(t[5]%2),14,0,14,14-4*(t[5]%2),14,0});
        lcd.pos(0, 6);
        lcd.lprint(center(padtwo(t[3])+"@7"+padtwo(t[4])+"@7"+padtwo(t[5])));
        lcd.pos(1,6);
        lcd.lprint(center(getd(t[6])));
        lcd.pos(2,6);
        lcd.lprint(center(getm(t[1])+" "+std::to_string(t[2])+ordate(t[2])));
        lcd.pos(3,6);
        lcd.lprint(center(std::to_string(t[0])));
    }
}

std::vector<std::pair<uint, uint>> Analog::line(int x1, int y1){
    std::vector<std::pair<uint, uint>> use;
    // Work in half-pixel units
    int x0 = 23;
    int y0 = 23;
    x1 *= 2;
    y1 *= 2;
    int dx = std::abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;
    while (true) {
        // Convert half-pixel coordinates to actual pixels
        uint x = x0 / 2;
        uint y = y0 / 2;
        use.push_back({x, y});
        if (x0 == x1 && y0 == y1){return use;}
        int e2 = 2 * error;
        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

std::vector<std::pair<uint,uint>> Analog::pixel_lines(uint minute, uint hour){
    int mradius = 11;
    int mangle = minute * 6 - 90;
    int mx = 11.5 + mradius * std::cos(mangle * M_PI / 180.0);
    int my = 11.5 + mradius * std::sin(mangle * M_PI / 180.0);
    auto muse = line(mx, my);
    int hradius = 7;
    int hangle = hour * 30 - 90;
    int hx = 11.5 + hradius * std::cos(hangle * M_PI / 180.0);
    int hy = 11.5 + hradius * std::sin(hangle * M_PI / 180.0);
    auto huse = line(hx, hy);
    std::vector<std::pair<uint, uint>> use = {{11,11},{11,12},{12,11},{12,12}};
    use.insert(use.end(), muse.begin(), muse.end());
    use.insert(use.end(), huse.begin(), huse.end());
    return use;
}

std::vector<std::array<uint, 3>> Analog::pix_set(const std::vector<std::pair<uint, uint>>& use){
    std::vector<std::array<uint, 3>> small;
    for (auto pixel : use) {
        std::array<uint, 2> block = {
            pixel.first / 5,
            pixel.second / 8
        };
        bool found = false;
        for (auto s : small) {
            if (s[0] == block[0] && s[1] == block[1]) {
                found = true;
                break;
            }
        }
        if (!found){small.push_back({block[0], block[1], 0});}
    }
    for (uint i = 0; i < small.size(); i++) {
        std::array<uint, 8> data = {0,0,0,0,0,0,0,0};
        for (auto pixel : use) {
            if (pixel.first / 5 == small[i][0]&&pixel.second / 8 == small[i][1]){
                data[pixel.second % 8] |= 1 << (4 - pixel.first % 5);
            }
        }
        if (i < 7) {
            small[i][2] = i;
            lcd.create_char(i, data);
        }
        else {
            //printf("Too many custom characters (%u)\n", i + 1);
        }
    }

    return small;
}

void Analog::lcd_print(std::vector<std::array<uint, 3>> chars) {
    std::string sndstr = "";
    for (uint r = 0; r < 3; r++) {
        lcd.pos(r, 0);
        sndstr = "";
        for (uint c = 0; c < 5; c++) {
            bool found = false;
            for (uint i = 0; i < chars.size(); i++) {
                if (c == chars[i][0] && r == chars[i][1]) {
                    sndstr += '@';
                    sndstr += std::to_string(chars[i][2]);
                    found = true;
                    break;
                }
            }
            if (!found){sndstr += ' ';}
        }
        sndstr += '\xFF';
        lcd.lprint(sndstr);
    }
    lcd.pos(3, 0);
    lcd.lprint("\xFF\xFF\xFF\xFF\xFF\xFF");
}

void Analog::analog(uint h, uint m){
    lcd_print(pix_set(pixel_lines(m,h)));
}

std::string Analog::center(const std::string& s,  const char c, uint n){
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

std::string Analog::ordate(uint n){
     if (11 <= n && n <= 13){return "th";}
    switch (n % 10) {
        case 1: return "st";
        case 2: return "nd";
        case 3: return "rd";
        default: return "th";
    }   
}

std::string Analog::getm(uint n){
    static const std::array<std::string, 12> months = {
        "January", "February", "March", "April",
        "May", "June", "July", "August",
        "September", "October", "November", "December"
    };

    return months[n-1];
}

std::string Analog::getd(uint n){
    static const std::array<std::string, 7> days = {
        "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday", "Sunday"
    };

    return days[n];   
}

std::string Analog::padtwo(int num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}