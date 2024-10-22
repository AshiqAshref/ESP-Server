#ifndef OUTPUT_H
#define OUTPUT_H
#include <Arduino.h>
#include <Error_Codes.h>


class Output {

public:
  Output() =default;
  static bool initializeOLED();

  static void print(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void OLED_print(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(int value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;

  static void OLED_print(const String &value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void OLED_println(const String &value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0) ;
  static void print(const String &value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println(const String &value, bool clear_line_ = true, int16_t y = -1, int16_t x = 0);
  static void println();
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
  static String get_error_as_string(INTERNAL_ERROR_CODE error_code);
  static void print_all_errors();
  static void print_error(INTERNAL_ERROR_CODE error_code);
  static void println_error(INTERNAL_ERROR_CODE error_code);
  static byte get_current_line();
  static byte get_line(int16_t cursor_y);
  static void set_cursor(int16_t cursor_x, int16_t cursor_y);
  static int16_t get_cursor_x();
  static int16_t get_cursor_y();
  static void set_line(byte line_no);

  static void animateConnection(byte &a);
  static void draw_Wifi_icon(byte strength);
  static void draw_SD_eror_icon(bool draw=true);
  static void draw_AP_active_icon(bool draw=true);
  static void clearBlock(byte x,  byte y, bool display=true);
  static void update_Wifi_RSSI_ICO(int RSSI);
};
#endif //OUTPUT_H
