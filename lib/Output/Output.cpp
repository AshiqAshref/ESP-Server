#include "Output.h"
#include <Adafruit_SSD1306.h>
#include <Error_Codes.h>
#include <RTClib.h>

extern Adafruit_SSD1306 oled;
extern Error_Codes error_codes;

static constexpr byte line_size=6;
static constexpr int16_t line[line_size]{16, 24, 32, 40, 48, 56};

void Output::OLED_print(const unsigned char value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const unsigned char value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.println(value);
        oled.display();
    }
}
void Output::print(const unsigned char value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.print(value);
    OLED_print(value, clear_line_, y, x);
}
void Output::println(const unsigned char value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.println(value);
    OLED_println(value, clear_line_, y, x);

}

void Output::OLED_print(const char value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const char value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.println(value);
        oled.display();
    }
}
void Output::print(const char value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.print(value);
    OLED_print(value, clear_line_, y, x);
}
void Output::println(const char value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.println(value);
    OLED_println(value, clear_line_, y, x);
}

void Output::OLED_print(const long value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const long value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.println(value);
        oled.display();
    }
}
void Output::print(const long value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.print(value);
    OLED_print(value, clear_line_, y, x);
}
void Output::println(const long value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.println(value);
    OLED_println(value, clear_line_, y, x);
}

void Output::OLED_print(const unsigned long value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const unsigned long value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.println(value);
        oled.display();
    }
}
void Output::print(const unsigned long value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.print(value);
    OLED_print(value, clear_line_, y, x);
}
void Output::println(const unsigned long value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.println(value);
    OLED_println(value, clear_line_, y, x);
}

void Output::OLED_print(const String &value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const String &value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.println(value);
        oled.display();
    }
}
void Output::print(const String &value, const bool clear_line_, const int16_t y , const int16_t x) {
    OLED_print(value, clear_line_, y, x);
    Serial.print(value);
}
void Output::println(const String &value, const bool clear_line_, const int16_t y , const int16_t x ) {
    OLED_println(value, clear_line_, y, x);
    Serial.println(value);
}
void Output::println() {
    OLED_println("", false);
    Serial.println("");
}

void Output::OLED_print(const int value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const int value, const bool clear_line_, const int16_t y , const int16_t x) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        if(y!=-1 || x!=0) {
            if(y<line_size) oled.setCursor(x,line[y]);
            else oled.setCursor(x,y);
        }
        if(oled.getCursorY()<line[0]) oled.setCursor(oled.getCursorX(),line[0]);
        else if(oled.getCursorY()>line[line_size-1]) clear_line(0);
        if(clear_line_)clear_line();
        oled.println(value);
        oled.display();
    }
}
void Output::print(const int value, const bool clear_line_, const int16_t y , const int16_t x) {
    Serial.print(value);
    OLED_print(value, clear_line_, y, x);
}
void Output::println(const int value, const bool clear_line_, const int16_t y, const int16_t x) {
    Serial.println(value);
    OLED_println(value, clear_line_, y, x);
}

void Output::printBin(const byte aByte, const int16_t y, const int16_t x) {
    print('(', y, x);
    for (int8_t aBit = 7; aBit >= 0; aBit--) {
        print(bitRead(aByte, aBit) ? '1' : '0');
        if(aBit==4) print(' ');
    }
    print(')');
}
void Output::printlnBin(const byte aByte, const int16_t y, const int16_t x) {
    printBin(aByte, y, x);
    println();
}

String Output::beautifyTime(const uint8_t h_m_s) {
    if(h_m_s<10)
        return '0'+static_cast<String>(h_m_s);
    return static_cast<String>(h_m_s);
}
String Output::get_formated_Time(const unsigned long unix_time, const byte mode) {
    const auto curr_time= DateTime(unix_time);
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

int16_t Output::clear_line(const int16_t y) {
    if(y>line_size-1)
        oled.setCursor(0,line[line_size-1]);
    else if(y<0) 
        oled.setCursor(0, line[get_current_line()]);
    else
        oled.setCursor(0,line[y]);
    const int16_t current_y = oled.getCursorY();
    const byte width = oled.width()-6;
    oled.setTextColor(BLACK);
    while(oled.getCursorX()<width) {
        oled.print(static_cast<char>(219));
    }
    oled.setTextColor(WHITE);
    oled.setCursor(0,current_y);
    return current_y;
}

void Output::clear_display() {
    oled.clearDisplay();
}
String Output::get_error_as_string(const INTERNAL_ERROR_CODE error_code) {
    if(error_code==BAD_WIFI_CRED)
        return "BAD_WIFI_CRED";
    if(error_code==WIFI_CONN_ERROR)
        return "WIFI_CONN_ERROR";
    if (error_code == SD_CARD_ERROR)
        return "SD_CARD_ERROR";
    if(error_code == SOFT_SERIAL_ERROR)
        return "SOFT_SERIAL_ERROR";
    if(error_code == SETTING_AP_FAIL)
        return "SETTING_AP_FAIL";
    if(error_code == OLED_ERROR)
        return "OLED_ERROR";
    if(error_code == MDNS_ERROR)
        return "MDNS_ERROR";
    if(error_code == NTP_ERROR)
        return "NTP_ERROR";
    return "INVALID ERROR CODDE";
}

void Output::print_error(const INTERNAL_ERROR_CODE error_code) {
    print(get_error_as_string(error_code));
}
void Output::println_error(const INTERNAL_ERROR_CODE error_code) {
    print_error(error_code);
    println();
}
void Output::print_all_errors() {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0){
        if(error_codes.total_errors()!=0) {
            println("Active_Errors: {");
            for(short i=0;i<error_codes.total_errors();i++) {
                println_error(error_codes.get_error(i));
            }
            println('}');
        }else {
            println("No active errors");
        }
    }
}

byte Output::get_current_line() {
    return get_line(oled.getCursorY());
}
byte Output::get_line(const int16_t cursor_y){
    if(cursor_y<0) return 0;
    for(int i=0; i<line_size-1;i++){
        if(cursor_y>=line[i])
            if(cursor_y<line[i+1]) return i;
    }
    if(cursor_y<oled.height()) return line_size-1;
    return 0;
}

void Output::set_line(const byte line_no) {
    oled.setCursor(0,line[line_no]);
}

void Output::set_cursor(const int16_t cursor_x, const int16_t cursor_y) {
    oled.setCursor(cursor_x, cursor_y);
}

int16_t Output::get_cursor_x() {
    return oled.getCursorX();
}
int16_t Output::get_cursor_y() {
    return oled.getCursorY();
}

void Output::animateConnection(byte &a) const {
    if(a>3) a = 0;
    draw_Wifi_icon(a++);
}

void Output::update_Wifi_RSSI_ICO(const int RSSI) const {
    if(RSSI>-56) {
        draw_Wifi_icon(3);
        return;
    }if(RSSI<-80) {
        draw_Wifi_icon(0);
        return;
    }if(RSSI<-70) {
        draw_Wifi_icon(1);
        return;
    }if(RSSI<-67) {
        draw_Wifi_icon(2);
    }
}

void Output::draw_Wifi_icon(const byte strength) const {//0,1,2,3=full,4=cross
    constexpr byte x=112, y=0;
    oled.drawBitmap(x, y, ClearMask, 16, 16, 0);
    if(strength<4){
        oled.drawBitmap(x, y, array_[strength], 16, 16, 1);
    }else if(strength==4){
        oled.drawBitmap(x, y, array_[4], 16, 16, 1);
        oled.drawBitmap(x, y, array_[2], 16, 16, 1);
    }
    oled.display();
}
void Output::draw_AP_active_icon(const bool draw) const {
    constexpr byte x=92, y=0;
    if(draw)
        oled.drawBitmap( x,  y, ap_ico, 16, 16, 1);
    else
        clearBlock(x,y,false);
    oled.display();
}
void Output::draw_SD_eror_icon(const bool draw) const {
    constexpr byte x=74, y=0;
    if(draw)
        oled.drawBitmap( x,  y, sd_ico, 16, 16, 1);
    else
        clearBlock(x,y,false);
    oled.display();
}


void Output::clearBlock(const byte x, const byte y, const bool display) const {
    oled.drawBitmap(x, y, ClearMask, 16, 16, 0);
    if(display)oled.display();
}
