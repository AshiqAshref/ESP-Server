#ifndef COMMUNICATION_PROTOCOLS_H
#define COMMUNICATION_PROTOCOLS_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Command.h>


constexpr unsigned long time_out_ = 5000;

class Communication_protocols {
protected:
    const unsigned long NTP_refresh_rate_= 3600000;

    [[nodiscard]] static COMM_PROTOCOL sendJsonDocument(const JsonDocument &doc, Command_enum command);
    static JsonDocument receive_jsonDocument(Command_enum command);
    static COMM_PROTOCOL sendLong(unsigned long res_long, Command_enum command);
    static unsigned long getLongFromBuffer(Command_enum command);

    static IPAddress receive_IP(Command_enum command);

    static COMM_PROTOCOL send_IP(const IPAddress &IP, Command_enum command);

    [[nodiscard]] static byte getCommand(byte response_header);
    static COMM_PROTOCOL send_response_READY_TO_SEND(Command_enum command,bool clear_buffer = true);
    static void send_response_READY_TO_RECV(Command_enum command);
    static byte getProtocol(byte b);
    static void send_header(Command_enum command, byte protocol_id);
    static void send_request_RETRY(Command_enum command);
    static void send_request_SYN(Command_enum command);
    static void send_status_SUCCESS(Command_enum command);
    static void send_status_UNKW_ERROR(Command_enum command);
    static void send_status_TIMEOUT(Command_enum command);
    static void send_response_ACK(Command_enum command);
    [[nodiscard]] static COMM_PROTOCOL send_response_SYN_ACK(Command_enum command, bool clear_buffer = true) ;
    static void close_session(Command_enum command);
    [[nodiscard]] static COMM_PROTOCOL get_response(Command_enum command, bool clear_buffer = true);

    static bool wait_for_response(Command_enum command);
    [[nodiscard]] static bool wait_for_response() ;
    static void clear_receive_buffer();

    static void printHeader(byte header);
    static String protocol_as_String(byte comm_protocol);
    static String command_as_String(byte command);


public:
    Communication_protocols()=default;
    static byte *longToByte(unsigned long long_);//delete val after use
    static unsigned long bytesToLong(const byte *byte_);
};
#endif //COMMUNICATION_PROTOCOLS_H
