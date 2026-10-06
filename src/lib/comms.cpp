#include "comms.hpp"

#include "pico/stdlib.h"
#include <cstddef>
#include <pico/error.h>
#include <string>
#include <array>
#include <charconv>

COMMS::COMMS():con(false),buffer(""),time({0,0,0,0,0,0,0}){}

bool COMMS::connected(){return con;}
void COMMS::disconnect(){con=false;}

void COMMS::send(const std::string& str){printf("%s\n", str.c_str());}
void COMMS::send(const char* str){printf("%s\n", str);}

std::string COMMS::readline(){
    while (true){
        int c=getchar_timeout_us(0);
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
    std::string line=readline();
    if (line==""){return;}
    if (line.substr(0,11)=="///TIMED///"){
        line.erase(0,11);
        const std::string delim="///SPLIT///";
        size_t pos;
        int i=0;
        while ((pos=line.find(delim))!=std::string::npos){
            if (i>=7){
                rtrars();
                return;
            }
            std::string sub=line.substr(0,pos);
            auto result=std::from_chars(sub.data(),sub.data()+sub.size(),time[i]);
            if (result.ec!=std::errc()||result.ptr!=sub.data()+sub.size()){
                rtrars();
                return;
            }
            line.erase(0,pos+delim.length());
            i++;
        }
        if (i!=7){
            rtrars();
            return;
        }
        send("PICO");
    }
    else if (line.substr(0,11)=="///CONFD///"){con=true;}
}

std::array<uint,7> COMMS::rtrars(){
    std::array<uint,7> cp=time;
    time={0,0,0,0,0,0,0};
    return cp;
}