#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <Error_Codes.h>
#include <FS.h>
#include <ReminderA.h>
#include <ReminderB.h>
#include <Wire.h>
#include <SPI.h>

bool initializeHardwareSerial();
bool initializeWiFi();
bool initializeMDNS();
bool initializeNTP();
bool initializeOLED();
void updateUiComponents();


// boolean initializeRTC();


void resolve_errors();

bool resolve_error(INTERNAL_ERROR_CODE error_code);

bool resolve_WIFI_CONN_ERROR();
bool resolve_SD_CARD_ERROR();
bool resolve_MDNS_ERROR();
bool resolve_BAD_WIFI_CRED();

void setAccessPoint();
ReminderA jsonToClass(String& dat);
ReminderA checkUpcomming(const DateTime& currentTime);
ReminderB jsonToClassB(const String& dat);
ReminderB checkUpcommingB(const DateTime& currentTime);
String handle_index_modeB(const String &remoteIp);
String handle_index(const String &remoteIp);
String getReminders();

#endif //MAIN_H
//SD MODULE:  D7 MOSI, D6 MISO, D5 CLK, D0 CS
//RTC MODULE D1 SCL, D2 SDA
//SOFT SERRIAL D4 TX, D8 RX
