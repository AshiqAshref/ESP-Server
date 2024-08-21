#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <SDFS.h>
#include <ReminderA.h>
#include <ReminderB.h>
#include <Wire.h>
#include <SPI.h>

boolean initializeHardwareSerial();

bool initializeSDFS();
boolean initializeWiFi();
bool initializeMDNS();

// boolean initializeRTC();
void writeFile(const char &path, const char *message, const char *M);
String readFile(const String& path);
String readLine(File file);
String readLine(File file, byte line_no);
String readLine(const String& path, byte line_no);
void save_wifi_cred(const String& ssid_, const String& pass_);
bool load_wifi_cred();
void printAll(const String& path);


void setAccessPoint();
ReminderA jsonToClass(String& dat);
ReminderA checkUpcomming(const DateTime& currentTime);
ReminderB jsonToClassB(const String& dat);
ReminderB checkUpcommingB(const DateTime& currentTime);
String handle_index_modeB(const String &remoteIp);
String handle_index(const String &remoteIp);
String getReminders();

String beautifyTime(uint8_t h_m_s);
String get_formated_Time(const DateTime &curr_time, byte mode=12);

#endif //MAIN_H
