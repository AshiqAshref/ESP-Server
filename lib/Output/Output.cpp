//
// Created by user on 27/08/2024.
//

#include "Output.h"

#include <Adafruit_SSD1306.h>
#include <Error_Codes.h>

extern Error_Codes error_codes;
extern Adafruit_SSD1306 oled;

void Output::OLED_print(const String &value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const String &value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.println(value);
        oled.display();
    }
}
void Output::OLED_print(const unsigned char value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const unsigned char value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.println(value);
        oled.display();
    }
}
void Output::OLED_print(const int value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const int value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.println(value);
        oled.display();
    }
}
void Output::OLED_print(const long value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const long value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.println(value);
        oled.display();
    }
}

void Output::OLED_print(const unsigned long value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.print(value);
        oled.display();
    }
}
void Output::OLED_println(const unsigned long value) {
    if(error_codes.check_if_error_exist(OLED_ERROR)<0) {
        oled.println(value);
        oled.display();
    }
}

void Output::print(const String &value) {
    Serial.print(value);
    OLED_print(value);
}
void Output::println(const String &value ) {
    Serial.println(value);
    OLED_println(value);
}

void Output::print(unsigned char value) {
    Serial.print(value);
    OLED_print(value);
}

void Output::println(unsigned char value) {
    Serial.println(value);
    oled.println(value);
}

void Output::print(const int value) {
    Serial.print(value);
    OLED_print(value);
}

void Output::println(const int value) {
    Serial.println(value);
    OLED_println(value);
}

void Output::print(const long value) {
    Serial.print(value);
    OLED_print(value);
}

void Output::println(const long value) {
    Serial.println(value);
    OLED_println(value);
}

void Output::print(const unsigned long value) {
    Serial.print(value);
    OLED_print(value);
}
void Output::println(const unsigned long value) {
    Serial.println(value);
    OLED_println(value);
}
