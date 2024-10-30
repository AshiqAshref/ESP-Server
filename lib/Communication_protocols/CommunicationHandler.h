
#ifndef COMMUNICATIONHANDLER_H
#define COMMUNICATIONHANDLER_H
#include <Arduino.h>
#include <Communication_protocols.h>
#include <esp_wifi_types.h>


class CommunicationHandler: public Communication_protocols{
    static COMM_PROTOCOL sendTime() ;
    static void handle_header(byte response_header) ;

    static void printmode(wifi_mode_t mode);
    static void printmode();

public:
    static bool NTP_request_handler();
    static bool reminder_b_request_handler();
    static bool activate_AP_request_handler();

    static void handle_communications();
    static bool initializeHardwareSerial();

    static void send_command_get_network_inf();
    static bool get_network_inf_response_handler();
    static bool get_network_inf_request_handler();

    static void send_command_deactivate_ap();
    static bool deactivate_AP_request_handler();
    static bool deactivate_AP_response_handler();

};



#endif //COMMUNICATIONHANDLER_H
