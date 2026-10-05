#include "comms.hpp"

#include "pico/stdlib.h"
#include <pico/error.h>
#include <string>
#include <array>

COMMS::COMMS():con(false),buffer(""){}

bool COMMS::connected(){return con;}

void COMMS::send(std::string& str){printf("%s\n", str.c_str());}

std::string COMMS::readline(){
    while (true){
        char c=getchar_timeout_us(0);
        if (c==PICO_ERROR_TIMEOUT){break;}
        else {buffer+=static_cast<char>(c);}
    }
    std::string str;
    for (int i=0; i<buffer.length(); i++){
        if (buffer[i]!='\n'){str+=static_cast<char>(buffer[i]);}
        else{
            buffer.erase(0,str.length()+1);
            return str;
        }       
    }

    return "";
}

void COMMS::connect(){
    con=false;
    std::string line;
    while (true){
        line=readline();
        if (line==""){break;}
    }
}