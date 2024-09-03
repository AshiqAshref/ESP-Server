#ifndef OUTPUT_H
#define OUTPUT_H
#include <Arduino.h>
#include <Error_Codes.h>

class Output {
unsigned char icoWifi_Full [32]  = {
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

unsigned char icoWifi3 [32] = {
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

unsigned char icoWifi2 [32]  = {
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

unsigned char icoWifi1 [32]  = {
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

unsigned char WifiCross_RightDiag [32] = {
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

  unsigned char sd_ico[32] ={
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

  unsigned char ap_ico[32] = {
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

unsigned char ClearMask [32] ={
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

static constexpr int icoallArray_LEN = 5;
const unsigned char* array_[icoallArray_LEN] = {
  icoWifi1, //0
  icoWifi2, //1
  icoWifi3, //2
  icoWifi_Full, //3
  WifiCross_RightDiag, //4
};

public:
  Output() =default;
  static void print(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void OLED_print(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;

  static void OLED_print(const String &value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(const String &value="", bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void print(const String &value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(const String &value="", bool clear_line_ = true, int16_t y = -1, int16_t x = 0);

  static void OLED_print(unsigned char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(unsigned char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void print(unsigned char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(unsigned char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);

  static void OLED_print(char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void print(char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(char value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);

  static void OLED_print(long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void print(long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);

  static void OLED_print(unsigned long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(unsigned long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void print(unsigned long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(unsigned long value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);

  static void printBin(byte aByte, int16_t y = -1, int16_t x = 0);
  static void printlnBin(byte aByte, int16_t y = -1, int16_t x = 0);
  static int16_t clear_line(int16_t y = -1);
  static void clear_display();

  static String beautifyTime(uint8_t h_m_s);
  static String get_formated_Time(unsigned long unix_time, byte mode = 12);

  static void print_all_errors();
  static void print_error(INTERNAL_ERROR_CODE error_code);
  static void println_error(INTERNAL_ERROR_CODE error_code);
  static byte get_current_line();
  static byte get_line(int16_t cursor_y);
  static void set_cursor(int16_t cursor_x, int16_t cursor_y);
  static int16_t get_cursor_x();
  static int16_t get_cursor_y();
  static void set_line(byte line_no);
  void animateConnection(byte &a) const;
  void draw_Wifi_icon(byte strength) const;
  void draw_SD_eror_icon(bool draw=true) const;
  void draw_AP_active_icon(bool draw=true) const;

  void clearBlock(byte x,  byte y, bool display=true) const;
  void update_Wifi_RSSI_ICO(int RSSI) const;
};
#endif //OUTPUT_H
