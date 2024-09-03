#include "Error_Codes.h"



void Error_Codes::add_error(const INTERNAL_ERROR_CODE error_code) {
    if(check_if_error_exist(error_code)==-1) {
        error_codes[total_active_errors_] = error_code;
        total_active_errors_++;
    }
}

INTERNAL_ERROR_CODE Error_Codes::get_error(const byte index) const {
    return error_codes[index];
}

byte Error_Codes::get_error_code(const byte index) const {
    return get_error(index);
}

int Error_Codes::check_if_error_exist(const INTERNAL_ERROR_CODE error_code) const {
    for(int i=0;i<total_active_errors_;i++)
        if(error_codes[i]==error_code)
            return i;
    return -1;
}

void Error_Codes::remove_error(const INTERNAL_ERROR_CODE error_code) {
    const int result = check_if_error_exist(error_code);
    if( result!=-1) {
        for(byte i = result; i < total_active_errors_; i++)
            if(i+1<total_active_errors_)
                error_codes[i]=error_codes[i+1];
        total_active_errors_--;
    }
}


String Error_Codes::get_error_as_string(const INTERNAL_ERROR_CODE error_code) {
    if(error_code==BAD_WIFI_CRED)
        return "BAD_WIFI_CRED";
    if(error_code==WIFI_CONN_ERROR)
        return "WIFI_CONN_ERROR";
    if (error_code == SD_CARD_ERROR)
        return "SD_CARD_ERROR";
    if(error_code == SOFT_SERIAL_ERROR)
        return "SOFT_SERIAL_ERROR";
    if(error_code == SETTING_AP_FAIL)
        return "SETTING_AP_FAIL";
    if(error_code == OLED_ERROR)
        return "OLED_ERROR";
    if(error_code == MDNS_ERROR)
        return "MDNS_ERROR";
    if(error_code == NTP_ERROR)
        return "NTP_ERROR";
    return "INVALID ERROR CODDE";
}



// void Error_Codes::print_all_errors() const {
//     if(total_active_errors_!=0) {
//         Serial.println("Active_Errors: {");
//         for(int i=0;i<total_active_errors_;i++)
//             println_error(error_codes[i]);
//         Serial.println('}');
//     }else
//         Serial.println("No active errors");
// }



