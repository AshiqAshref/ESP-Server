#ifndef AV_FUNCTIONS_H
#define AV_FUNCTIONS_H
#include <Arduino.h>
#include <RTClib.h>
#include <ArduinoJson.h>

class AV_Functions {
    static JsonDocument unsimplify_Json_local(const JsonDocument &doc);
    static JsonDocument simplify_Json_local(const JsonDocument &doc);


public:
    AV_Functions()=default;
    ~AV_Functions()=default;
    static void waitForInput();
    static void flush_();

    static String get_formated_Time(const DateTime &curr_time, byte mode=12);
    static String beautifyTime(uint8_t h_m_s);
    static void printBin(byte aByte);
    static void printlnBin(byte aByte);

    static bool isJsonValid(const JsonDocument &doc);

    static JsonDocument simplify_Json(JsonDocument doc);
    static JsonDocument unsimplify_Json(JsonDocument doc);

    static byte extractHour(const String &formated_time);

    static byte extractMinute(const String &formated_time);
};
#endif //AV_FUNCTIONS_H
