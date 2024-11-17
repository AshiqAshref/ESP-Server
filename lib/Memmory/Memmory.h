#ifndef MEMMORY_H
#define MEMMORY_H
#include<Arduino.h>
#include <FS.h>
#include <RTClib.h>
#include <ArduinoJson.h>

const String WIFI_SSID_JSON_KEY = "wifi_SSID";
const String WIFI_PASS_JSON_KEY = "wifi_PASS";
const String wifiConfigFile = "/WifiConfig.dat";
const String reminderBfile="/ReminderB.dat";
const String daylight_saving_file="/DLT_SV.dat";
const String server_ip_file="/SRV_IP.dat";
const String reminder_b_revision_file="/B_REV.dat";

class Memmory {
static JsonDocument get_reminder(JsonDocument &json_array, byte position);

public:
    explicit Memmory();
    static bool initializeSDFS();

    static bool get_daylight_saving();
    static void save_daylight_saving(bool dlt_sv);
    static IPAddress get_server_ip();
    static void save_server_ip(const IPAddress &server_ip);
    static unsigned long get_reminder_b_revision_no();
    static void save_reminder_b_revision_no(unsigned long revision_no);
    static void save_wifi_cred(const String& ssid_, const String& pass_) ;
    static bool load_wifi_cred(String &WIFI_SSID, String &WIFI_PASS) ;
    static JsonDocument get_latest_Reminder(const String &time_string, JsonDocument &doc);
    static JsonDocument get_latest_Reminder(unsigned long unixTime, JsonDocument &doc);

    static JsonDocument get_latest_Reminder(unsigned long unixTime);
    static JsonDocument get_latest_Reminder(const DateTime &t, JsonDocument &doc);
    static JsonDocument get_all_reminders_from_sd() ;


    static String readFile(const String& path);
    static String readLine(File file);
    static String readLine(File file, byte line_no);
    static String readLine(const String& path, byte line_no);
    static void writeFile(const String &path, const char *message, const char *mode);
    static bool write_reminders_to_SD(const JsonDocument &reminders_json);
    static void sd_print_all_files(const String& path);


    static String timetoString(const DateTime& t);
    static bool resolve_SD_CARD_ERROR();
};
#endif //MEMMORY_H
