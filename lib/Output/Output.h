//
// Created by user on 27/08/2024.
//

#ifndef OUTPUT_H
#define OUTPUT_H
#include <Arduino.h>




class Output {
public:
    Output()=default;
    static void OLED_print(const String &value);
    static void OLED_println(const String &value="");
    static void OLED_print(unsigned char value);
    static void OLED_println(unsigned char value);
    static void OLED_print(int value);
    static void OLED_println(int value);
    static void OLED_print(long value);
    static void OLED_println(long value);
    static void OLED_print(unsigned long value);
    static void OLED_println(unsigned long value);

    static void print(const String &value);
    static void println(const String &value="");
    static void print(unsigned char value);
    static void println(unsigned char value);
    static void print(int value);
    static void println(int value);
    static void print(long value);
    static void println(long value);
    static void print(unsigned long value);
    static void println(unsigned long value);

};



#endif //OUTPUT_H
