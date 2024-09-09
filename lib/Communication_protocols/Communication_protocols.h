#ifndef COMMUNICATION_PROTOCOLS_H
#define COMMUNICATION_PROTOCOLS_H
#include <Arduino.h>

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

class Communication_protocols {
    const unsigned long time_out_ = 10000;
    const unsigned long NTP_refresh_rate_= 3600000;
    const byte funct_id_getTime_ =   0b00001000;
    const byte function_id_filter_ = 0b00001111;

    [[nodiscard]] COMM_PROTOCOL sendTime() const;
    [[nodiscard]] COMM_PROTOCOL get_response(byte function_id) const;
    static void close_session(byte function_id);
    void handle_header(byte response_header) const;
    [[nodiscard]] bool handleNTPcommand(byte max_retries = 20) const;

    static void clear_receive_buffer();
    static void send_header(byte function_id, byte protocol_id);
    [[nodiscard]] bool wait_for_response() const;
    static void sendLong(unsigned long res_long);
    static void send_request_RETRY(byte function_id);
    static void send_request_SYN(byte function_id);
    static void send_status_SUCCESS(byte function_id);
    static void send_status_TIMEOUT(byte function_id);
    [[nodiscard]] COMM_PROTOCOL send_response_SYN_ACK(byte function_id) const;
    bool getTimeFromBuffer();

public:
    Communication_protocols()=default;
    void handle_communications() const;
    static byte *longToByte(unsigned long long_);//delete val after use
    static unsigned long bytesToLong(const byte *byte_);
    static byte getProtocol(byte b);
    [[nodiscard]] byte getFunction_id(byte response_header) const;
    static void printBin(byte aByte);
    static void printlnBin(byte aByte);
};
#endif //COMMUNICATION_PROTOCOLS_H
