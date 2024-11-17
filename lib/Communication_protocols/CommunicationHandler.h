
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

    static void send_command_daylight_sav();
    static bool daylight_sav_response_handler(bool daylight_sav, bool command_type);
    static bool daylight_sav_request_handler();
    static bool daylight_sav_receive_dls();
    static bool daylight_sav_send_dls(bool daylight_sav);

    static void send_command_server_ip();
    static bool server_ip_response_handler(const IPAddress &server_ip, bool command_type);
    static bool server_ip_request_handler();
    static bool server_ip_receive_ip();
    static bool server_ip_send_ip(const IPAddress &server_ip);

    static void send_command_deactivate_ap();
    static bool deactivate_AP_request_handler();
    static bool deactivate_AP_response_handler();

    static void send_command_reminderB_change();
    static bool reminderB_change_response_handler();

    static void send_command_reminderB_send_log();
    static bool reminderB_send_log_response_handler();
    static bool reminderB_send_log_request_handler();

    static bool reminderB_change_request_handler();



};



#endif //COMMUNICATIONHANDLER_H
