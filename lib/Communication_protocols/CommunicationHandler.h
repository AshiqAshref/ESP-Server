
#ifndef COMMUNICATIONHANDLER_H
#define COMMUNICATIONHANDLER_H
#include <Arduino.h>
#include <Communication_protocols.h>
#include <ArduinoJson.h>
#include <RTClib.h>


enum CommandStatus:byte {
    IN_PROGRESS = 0b10101010,
    FAILED = 0b01010101,
    COMPLETED = 0b00111100,
};

class CommunicationHandler: public Communication_protocols{
    [[nodiscard]] static bool NTP_request_handler(Commands command);
    static static bool activate_AP_request_handler(Commands command, byte max_retries=20);

    static bool deactivate_AP_request_handler(Commands command,  byte max_retries=20);


    static bool deactivate_AP_response_handler(Commands command);

    [[nodiscard]] static bool get_reminder_b_request_handler(Commands command, byte max_retries = 20);
    [[nodiscard]] static COMM_PROTOCOL sendTime(Commands command) ;


    static void handle_header(byte response_header) ;


public:
    static void handle_communications();
    static bool initializeHardwareSerial();
    static void send_command_deactivate_ap();








};



#endif //COMMUNICATIONHANDLER_H
