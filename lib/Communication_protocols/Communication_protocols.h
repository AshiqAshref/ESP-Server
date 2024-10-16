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

constexpr byte funct_id_getTime_       = 0b00001000;
constexpr byte funct_id_get_reminderB_ = 0b00001100;
constexpr byte function_id_filter_     = 0b00001111;
constexpr unsigned long time_out_ = 10000;

class Communication_protocols {
    const unsigned long NTP_refresh_rate_= 3600000;

    [[nodiscard]] static COMM_PROTOCOL sendTime(const byte function_id) ;
    [[nodiscard]] static COMM_PROTOCOL sendJsonDocument(const JsonDocument &doc, byte function_id, byte max_retries=20);

    static JsonDocument receive_jsonDocument(byte function_id, byte max_retries=20);

    [[nodiscard]] static COMM_PROTOCOL get_response(byte function_id, bool clear_buffer = true);
    static void close_session(byte function_id);
    static void handle_header(byte response_header) ;
    [[nodiscard]] static bool NTP_request_handler(const byte function_id, byte max_retries = 20);
    [[nodiscard]] static bool get_reminder_b_request_handler(byte function_id, byte max_retries = 20);

    static void clear_receive_buffer();
    static void send_header(byte function_id, byte protocol_id);
    [[nodiscard]] static bool wait_for_response() ;

    static bool wait_for_response(byte function_id);

    static COMM_PROTOCOL sendLong(unsigned long res_long, const byte function_id);
    static void send_request_RETRY(byte function_id);
    static void send_request_SYN(byte function_id);
    static void send_status_SUCCESS(byte function_id);
    static void send_status_UNKW_ERROR(byte function_id);
    static void send_status_TIMEOUT(byte function_id);
    static void send_response_ACK(byte function_id);
    [[nodiscard]] static COMM_PROTOCOL send_response_SYN_ACK(byte function_id, bool clear_buffer = true) ;

    static unsigned long getLongFromBuffer(const byte function_id);

    static JsonDocument simplify_Json(JsonDocument doc);
    static JsonDocument unsimplify_Json(JsonDocument doc);

public:
    Communication_protocols()=default;
    static void handle_communications() ;
    static byte *longToByte(unsigned long long_);//delete val after use
    static unsigned long bytesToLong(const byte *byte_);
    static byte getProtocol(byte b);
    [[nodiscard]] static byte getFunction_id(byte response_header);
    static void printBin(byte aByte);
    static void printlnBin(byte aByte);



};
#endif //COMMUNICATION_PROTOCOLS_H
