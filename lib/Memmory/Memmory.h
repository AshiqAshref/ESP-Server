#ifndef MEMMORY_H
#define MEMMORY_H
#include<Arduino.h>
#include <FS.h>
#include <RTClib.h>
#include <ArduinoJson.h>


class Memmory {
    const String WIFI_SSID_JSON_KEY = "wifi_SSID";
    const String WIFI_PASS_JSON_KEY = "wifi_PASS";
    String wifiConfigFile;
    String reminderBfile;

public:
    explicit Memmory(const String &wifiConfigFile);

    static bool initializeSDFS();
    static void writeFile(const String &path, const char *message, const char *mode);
    static String readFile(const String& path);
    static String readLine(File file);
    static String readLine(File file, byte line_no);
    static String readLine(const String& path, byte line_no);
    static void sd_print_all_files(const String& path);
    void save_wifi_cred(const String& ssid_, const String& pass_) const;
    bool load_wifi_cred(String &WIFI_SSID, String &WIFI_PASS) const;

    JsonDocument get_latest_Reminder(const String &time_string, JsonDocument &doc);
    JsonDocument get_latest_Reminder(unsigned long unixTime, JsonDocument &doc);
    JsonDocument get_latest_Reminder(const DateTime &t, JsonDocument &doc);
    JsonDocument get_reminder(JsonDocument &json_array, byte position);
    void write_reminders_to_SD(JsonDocument doc);
    JsonDocument get_all_reminders_from_sd() const;

    long fromFormatedTime(const String &reminder_time_col);

    static String timetoString(const DateTime& t);







};
#endif //MEMMORY_H
