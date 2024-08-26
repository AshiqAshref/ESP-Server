#include "Error_Codes.h"

#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 oled;

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


String Error_Codes::get_error(const INTERNAL_ERROR_CODE error_code) {
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
    return "INVALID ERROR CODDE";
}


void Error_Codes::print_error(const INTERNAL_ERROR_CODE error_code) {
    Serial.print(get_error(error_code));
}

void Error_Codes::println_error(const INTERNAL_ERROR_CODE error_code) {
    print_error(error_code);
    Serial.println();
}


void Error_Codes::print_all_errors_OLED() const {
    if(check_if_error_exist(OLED_ERROR)<0){
        oled.clearDisplay();
        oled.setTextSize(1);
        oled.setTextColor(WHITE);
        oled.setCursor(5, 5);
        if(total_active_errors_!=0) {
            oled.println("Active_Errors: {");
            for(short i=0;i<total_active_errors_;i++) {
                oled.println(get_error(error_codes[i]));
                oled.display();
            }
            oled.setCursor(5, static_cast<int16_t>((total_active_errors_+1)*8));
            oled.println('}');
        }else
            oled.println("No active errors");
        oled.display();
    }
}

void Error_Codes::print_all_errors() const {
    if(total_active_errors_!=0) {
        Serial.println("Active_Errors: {");
        for(int i=0;i<total_active_errors_;i++)
            println_error(error_codes[i]);
        Serial.println('}');
    }else
        Serial.println("No active errors");
}



