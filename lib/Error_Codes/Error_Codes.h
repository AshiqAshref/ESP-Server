#ifndef ERROR_CODES_H
#define ERROR_CODES_H
#include <Arduino.h>

enum INTERNAL_ERROR_CODE:byte {
    BAD_WIFI_CRED     =    0b00000001,
    WIFI_CONN_ERROR   =    0b00000010,
    SD_CARD_ERROR     =    0b00000011,
    SOFT_SERIAL_ERROR =    0b00000100,
    SETTING_AP_FAIL   =    0b00000101,
    OLED_ERROR        =    0b00000110,
    MDNS_ERROR        =    0b00000111,
    NTP_ERROR         =    0b00001000,
    SERVER_ERROR      =    0b00001100,
};

class Error_Codes {
    byte total_active_errors_ = 0;
    constexpr static byte totalerrorcodes=9;
    INTERNAL_ERROR_CODE error_codes[totalerrorcodes] = {};
public:
    Error_Codes()=default;
    void add_error(INTERNAL_ERROR_CODE error_code);
    void remove_error(INTERNAL_ERROR_CODE error_code);
    static String get_error_as_string(INTERNAL_ERROR_CODE error_code);

    [[nodiscard]] INTERNAL_ERROR_CODE get_error(byte index) const;
    [[nodiscard]] byte get_error_code(byte index) const;
    [[nodiscard]] bool check_if_error_exist(INTERNAL_ERROR_CODE error_code) const;

    int get_error_index(INTERNAL_ERROR_CODE error_code) const;

    [[nodiscard]] byte total_errors() const {return total_active_errors_;}

    // static String get_error_as_string(INTERNAL_ERROR_CODE error_code);

    // void print_all_errors_OLED() const;


};

#endif //ERROR_CODES_H
