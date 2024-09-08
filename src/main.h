#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
// #include "FS.h"
#include <Error_Codes.h>
#include <FS.h>
#include <ReminderA.h>
#include <ReminderB.h>
#include <Wire.h>
#include <SPI.h>

bool initializeHardwareSerial();
bool initializeSDFS();
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
bool initializeWiFi();
bool initializeMDNS();
bool initializeNTP();
bool initializeOLED();
void updateUiComponents();


// boolean initializeRTC();
void writeFile(const char &path, const char *message, const char *M);
String readFile(const String& path);
String readLine(File file);
String readLine(File file, byte line_no);
String readLine(const String& path, byte line_no);
void save_wifi_cred(const String& ssid_, const String& pass_);


bool load_wifi_cred();
void sd_print_all_files(const String& path);
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
