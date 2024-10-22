#ifndef COMMUNICATION_PROTOCOLS_H
#define COMMUNICATION_PROTOCOLS_H
#include <Arduino.h>
#include <ArduinoJson.h>

enum COMM_PROTOCOL:byte {
    SYN =             0b10000000,
    SYN_ACK =         0b01000000,
    ACK =             0b11000000,
    FIN =             0b00100000,
    RETRY =           0b01100000,
    TIMEOUT=          0b10100000,
    UNKW_ERR=         0b01010000,
    SUCCESS=          0b01110000,
    PROTOCOL_FILTER = 0b11110000
};


enum Commands:byte {
    GET_TIME = 0b00001000,
    GET_REMINDER_B =  0b00001100,
    ACTIVATE_AP = 0b00000100,
    DEACTIVATE_AP =     0b00000010,
    COMMAND_FILTER = 0b00001111
};


constexpr byte function_id_filter_     = 0b00001111;
constexpr unsigned long time_out_ = 10000;

class Communication_protocols {
protected:
    const unsigned long NTP_refresh_rate_= 3600000;


    [[nodiscard]] static COMM_PROTOCOL sendJsonDocument(const JsonDocument &doc, Commands command, byte max_retries=20);
    static JsonDocument receive_jsonDocument(Commands command, byte max_retries=20);
    static COMM_PROTOCOL sendLong(unsigned long res_long, Commands command);
    static unsigned long getLongFromBuffer(Commands command);

    static IPAddress receive_IP(Commands command, byte max_retries);

    static COMM_PROTOCOL send_IP(const IPAddress &IP, Commands command);

    [[nodiscard]] static byte getCommand(byte response_header);
    static byte getProtocol(byte b);
    static void send_header(byte command, byte protocol_id);
    static void send_request_RETRY(Commands command);
    static void send_request_SYN(Commands command);
    static void send_status_SUCCESS(Commands command);
    static void send_status_UNKW_ERROR(Commands command);
    static void send_status_TIMEOUT(Commands command);
    static void send_response_ACK(Commands command);
    [[nodiscard]] static COMM_PROTOCOL send_response_SYN_ACK(Commands command, bool clear_buffer = true) ;
    static void close_session(Commands command);
    [[nodiscard]] static COMM_PROTOCOL get_response(Commands command, bool clear_buffer = true);

    static bool wait_for_response(Commands command);
    [[nodiscard]] static bool wait_for_response() ;
    static void clear_receive_buffer();

    static JsonDocument simplify_Json(JsonDocument doc);
    static JsonDocument unsimplify_Json(JsonDocument doc);

public:
    Communication_protocols()=default;
    static byte *longToByte(unsigned long long_);//delete val after use
    static unsigned long bytesToLong(const byte *byte_);
};
#endif //COMMUNICATION_PROTOCOLS_H
