#include "Output.h"
#include <Adafruit_SSD1306.h>
#include <Error_Codes.h>
#include <RTClib.h>



extern Adafruit_SSD1306 oled;
extern Error_Codes error_codes;


constexpr unsigned char icoWifi_Full [32]  = {
  0b00000111, 0b11100000,//     ######
  0b00011111, 0b11111000,//   ##########
  0b00111100, 0b00111100,//  ####    ####
  0b01110000, 0b00001110,// ###        ###
  0b11100011, 0b11000111,//###   ####   ###
  0b11001111, 0b11110011,//##  ########  ##
  0b00011110, 0b01111000,//   ####  ####
  0b00111000, 0b00011100,//  ###      ###
  0b00110001, 0b10001100,//  ##   ##   ##
  0b00000111, 0b11100000,//     ######
  0b00001110, 0b01110000,//    ###  ###
  0b00001100, 0b00110000,//    ##    ##
  0b00000000, 0b00000000,//    #      #
  0b00000001, 0b10000000,//       ##
  0b00000001, 0b10000000,//       ##
  0b00000000, 0b00000000,
};

constexpr unsigned char icoWifi3 [32] = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000011, 0b11000000,//      ####
  0b00001111, 0b11110000,//    ########
  0b00011110, 0b01111000,//   ####  ####
  0b00111000, 0b00011100,//  ###      ###
  0b00110001, 0b10001100,//  ##   ##   ##
  0b00000111, 0b11100000,//     ######
  0b00001110, 0b01110000,//    ###  ###
  0b00001100, 0b00110000,//    ##    ##
  0b00000000, 0b00000000,//    #      #
  0b00000001, 0b10000000,//       ##
  0b00000001, 0b10000000,//       ##
  0b00000000, 0b00000000,//
};

constexpr unsigned char icoWifi2 [32]  = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000001, 0b10000000,//       ##
  0b00000111, 0b11100000,//     ######
  0b00001110, 0b01110000,//    ###  ###
  0b00001100, 0b00110000,//    ##    ##
  0b00000000, 0b00000000,//    #      #
  0b00000001, 0b10000000,//       ##
  0b00000001, 0b10000000,//       ##
  0b00000000, 0b00000000,//
};

constexpr unsigned char icoWifi1 [32]  = {
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000001, 0b10000000,//       ##
  0b00000001, 0b10000000,//       ##
  0b00000000, 0b00000000,
};

constexpr unsigned char WifiCross_RightDiag [32] = {
  0b00011111, 0b11111000,//    ##########
  0b00111000, 0b00011100,//   ###      ###
  0b01100000, 0b00000110,//  ##          ##
  0b11000000, 0b00001111,// ##          ####
  0b11000000, 0b00011011,// ##         ## ##
  0b10000000, 0b00110001,// #         ##   #
  0b10000000, 0b01100001,// #        ##    #
  0b10000000, 0b11000001,// #       ##     #
  0b10000001, 0b10000001,// #      ##      #
  0b10000011, 0b00000001,// #     ##       #
  0b10000110, 0b00000001,// #    ##        #
  0b11001100, 0b00000011,// ##  ##        ##
  0b11011000, 0b00000011,// ## ##         ##
  0b01110000, 0b00000110,//  ###         ##
  0b00111000, 0b00011100,//   ###      ###
  0b00011111, 0b11111000,//    ##########
};

constexpr unsigned char sd_ico[32] ={
    0b00001111, 0b11111110, //    ###########
    0b00011001, 0b00100110, //   ##  #  #  ##
    0b00011001, 0b00100110, //   ##  #  #  ##
    0b00011001, 0b00100110, //   ##  #  #  ##
    0b00011001, 0b00100110, //   ##  #  #  ##
    0b00011111, 0b11111110, //   ############
    0b00111110, 0b00111110, //  #####   #####
    0b01111110, 0b00111110, // ######   #####
    0b01111110, 0b00111110, // ######   #####
    0b01111110, 0b00111110, // ######   #####
    0b01111110, 0b00111110, // ######   #####
    0b01111111, 0b11111110, // ##############
    0b01111111, 0b11111110, // ##############
    0b01111110, 0b00111110, // ######   #####
    0b01111110, 0b00111110, // ######   #####
    0b01111111, 0b11111110, // ##############
    };

constexpr unsigned char ap_ico[32] = {
    0b00100000, 0b00000100, //  #          #
    0b01100000, 0b00000110, // ##          ##
    0b01100000, 0b00000110, // ##          ##
    0b11100100, 0b00100011, //###  #    #   ##
    0b11001100, 0b00110011, //##  ##    ##  ##
    0b11011000, 0b00011011, //## ##      ## ##
    0b11011001, 0b10011011, //## ##  ##  ## ##
    0b11011011, 0b11011011, //## ## #### ## ##
    0b11011011, 0b11011011, //## ## #### ## ##
    0b11011001, 0b10011011, //## ##  ##  ## ##
    0b11011000, 0b00011011, //## ##      ## ##
    0b11001100, 0b00110011, //##  ##    ##  ##
    0b11000100, 0b00100011, //##   #    #   ##
    0b01100000, 0b00000110, // ##          ##
    0b01100000, 0b00000110, // ##          ##
    0b00100000, 0b00000100, //  #          #
    };

constexpr unsigned char ClearMask [32] ={
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
  0b11111111, 0b11111111,//#################
};

constexpr int icoallArray_LEN = 5;
const unsigned char *array_[] = {
  icoWifi1, //0
  icoWifi2, //1
  icoWifi3, //2
  icoWifi_Full, //3
  WifiCross_RightDiag, //4
};



static constexpr byte line_size=6;
static constexpr int16_t line[line_size]{16, 24, 32, 40, 48, 56};

bool Output::initializeOLED() {
    if(oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        oled.cp437();
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setTextColor(WHITE);
        println("Start");
        error_codes.remove_error(OLED_ERROR);
        return true;
    }
    error_codes.add_error(OLED_ERROR);
    return false;
}


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

void Output::animateConnection(byte &a) {
    if(a>3) a = 0;
    draw_Wifi_icon(a++);
}

void Output::update_Wifi_RSSI_ICO(const int RSSI) {
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

void Output::draw_Wifi_icon(const byte strength) {//0,1,2,3=full,4=cross
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
void Output::draw_AP_active_icon(const bool draw) {
    constexpr byte x=92, y=0;
    if(draw)
        oled.drawBitmap( x,  y, ap_ico, 16, 16, 1);
    else
        clearBlock(x,y,false);
    oled.display();
}
void Output::draw_SD_eror_icon(const bool draw) {
    constexpr byte x=74, y=0;
    if(draw)
        oled.drawBitmap( x,  y, sd_ico, 16, 16, 1);
    else
        clearBlock(x,y,false);
    oled.display();
}

void Output::clearBlock(const byte x, const byte y, const bool display) {
    oled.drawBitmap(x, y, ClearMask, 16, 16, 0);
    if(display)oled.display();
}
