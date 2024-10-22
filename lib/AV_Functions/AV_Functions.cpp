#include "AV_Functions.h"

#include <RTClib.h>


void AV_Functions::waitForInput() {
    flush_();
    while(!Serial.available()) {}
    Serial.flush();
    while(Serial.available())Serial.read();
}

 void AV_Functions::flush_() {
    Serial.flush();
    while(Serial.available())Serial.read();
}

String AV_Functions::get_formated_Time(const DateTime &curr_time, const byte mode) {
     if(mode == 12)
         return
             beautifyTime(curr_time.twelveHour())+":"
             +beautifyTime(curr_time.minute())+":"
             +beautifyTime(curr_time.second())+" "
             +(curr_time.isPM()? "p": " ");
     return
         beautifyTime(curr_time.hour())+":"
         +beautifyTime(curr_time.minute()) +":"
         +beautifyTime(curr_time.second());
 }

String AV_Functions::beautifyTime(const uint8_t h_m_s) {
     if(h_m_s<10)
         return '0'+static_cast<String>(h_m_s);
     return static_cast<String>(h_m_s);
 }


void AV_Functions::printBin(const byte aByte) {
    Serial.print('(');
    for (int8_t aBit = 7; aBit >= 0; aBit--) {
        Serial.print(bitRead(aByte, aBit) ? '1' : '0');
        if(aBit==4) Serial.print(' ');
    }
    Serial.print(')');
}
void AV_Functions::printlnBin(const byte aByte) {
    printBin(aByte);
    Serial.println();
}





