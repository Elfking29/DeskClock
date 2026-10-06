#include "life.hpp"

#include "pico/rand.h"

#include <string>
#include <array>

Life::Life(DS1306& rtc, PCF8574& lcd):Screen(rtc, lcd){}

bool Life::check() {return true;}

void Life::setup() {
    lcd.create_char(0,{31,31,31,31,0,0,0,0});
    lcd.create_char(1,{0,0,0,0,31,31,31,31});
    lcd.create_char(2,{28,28,28,28,28,28,28,28});
    randSet(randint(lw,hi));
}

void Life::disconnect() {}

void Life::update(std::string key){}

uint Life::randint(uint l, uint h){return get_rand_32()%(h-l+1)+l;}

void Life::randSet(uint percent){
    for (int r=0; r<rows; r++){
        for (int c=0; c<cols; c++){
            board[r][c]=randint(0,100)<percent;
        }
    }
}

int Life::checkCell(uint r, uint c) {
    int neighbors = -board[r][c];
    for (uint i = std::max(0, (int)r - 1);
         i < std::min(r + 2, rows);
         i++) {
        for (uint j = std::max(0, (int)c - 1);
             j < std::min(c + 2, cols);
             j++) {
            neighbors += board[i][j];
        }
    }
    if (!board[r][c])
        return neighbors == 3 ? 1 : 0;
    return (neighbors >= 2 && neighbors <= 3) ? 1 : 0;
}

void Life::runCycle(){
    bool newboard[rows/2*2][cols];
    for (int r=0; r<rows; r++){
        for (int c=0; c<cols; c++){
            newboard[r][c]=checkCell(r,c);
        }
    }
    for (int r=0; r<rows; r++){
        for (int c=0; c<cols; c++){
            board[r][c]=newboard[r][c];
        }
    }
}

void Life::boardFix(){
    for (int r=0; r<rows; r+=2){
        for (int c=0; c<cols; c++){
            uint top = board[r][c];
            uint bottom = r+1 < rows ? board[r+1][c] : 0;
            fixboard[r/2][c] = top + 2 * bottom;
        }
    }
}

std::string Life::padtwo(uint num) {
    if (num < 10)
        return "0" + std::to_string(num);
    return std::to_string(num);
}

void Life::display(){
    boardFix();
    std::array <uint,7> t = rtc.get_time();
    lcd.home();
    for (int r=0; r<(rows+1)/2; r++){
        if (r==0){lcd.lprint(padtwo(t[3]));lcd.lprint("@2");}
        else if (r==1){lcd.lprint(padtwo(t[4]));lcd.lprint("@2");}
        else if (r==2){lcd.lprint(padtwo(t[5]));lcd.lprint("@2");}
        else if (r==3){lcd.lprint("\xFF\xFF");lcd.lprint("@2");}
        for (int c=0; c<cols; c++){
            if (fixboard[r][c]==3){lcd.lprint("\xFF");}
            else if (fixboard[r][c]==2){lcd.lprint("@1");}
            else if (fixboard[r][c]==1){lcd.lprint("@0");}
            else {lcd.lprint(" ");}
        }
    }
    runCycle();
    bool equal=true;
    for (int r=0; r<rows; r++){
        for (int c=0; c<cols; c++){
            if (board[r][c]!=oldboard[r][c]){equal=false;}
        }
    }
    if (equal){repeat++;}
    else{
        repeat=0;
        for (int r=0; r<rows; r++){
            for (int c=0; c<cols; c++){
                oldboard[r][c]=board[r][c];
            }
        }       
    }
    if (repeat>2){randSet(randint(lw, hi));}
}
