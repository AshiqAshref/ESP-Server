#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <Error_Codes.h>
#include <ReminderA.h>
#include <ReminderB.h>
#include <Wire.h>
#include <SPI.h>

void updateUiComponents();

void resolve_errors();
bool resolve_error(INTERNAL_ERROR_CODE error_code);

String getReminders();

#endif //MAIN_H
//SD MODULE:  D7 MOSI, D6 MISO, D5 CLK, D0 CS
//RTC MODULE D1 SCL, D2 SDA
//SOFT SERRIAL D4 TX, D8 RX
