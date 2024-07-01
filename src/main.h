#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <ReminderA.h>
#include <ReminderB.h>

boolean initializeHardwareSerial();
void initializeSDFS();
boolean initializeWiFi();
boolean initializeRTC();
void writeFile(const char &path, const char *message, const char *M);
String readFile(const String& path);
void setAccessPoint();
ReminderA jsonToClass(String& dat);
ReminderA checkUpcomming(const DateTime& currentTime);
ReminderB jsonToClassB(const String& dat);
ReminderB checkUpcommingB(const DateTime& currentTime);
String handle_index_modeB(const String &remoteIp);
String handle_index(const String &remoteIp);

#endif //MAIN_H
