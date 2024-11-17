#include "Communication_protocols.h"

#include <NTPClient.h>
#include <CrcWriter.h>
#include <Memmory.h>
#include <FastCRC.h>

constexpr byte max_retries=20;

byte Communication_protocols::getProtocol(const byte b) {
    return b & PROTOCOL_FILTER;
}
byte Communication_protocols::getCommand(const byte response_header)  {
    return response_header & COMMAND_FILTER;
}

byte *Communication_protocols::longToByte(const unsigned long long_) {
    const auto a_arr = new byte[4]  {
        static_cast<byte>(long_ >> 24),
        static_cast<byte>(long_ >> 16),
        static_cast<byte>(long_ >> 8),
        static_cast<byte>(long_)
        };
    return a_arr;
}
unsigned long Communication_protocols::bytesToLong(const byte *byte_) {
    return
        (static_cast<long>(byte_[0])<<24) |
        (static_cast<long>(byte_[1])<<16) |
        (static_cast<long>(byte_[2])<<8)  |
        (static_cast<long>(byte_[3]));
}

COMM_PROTOCOL Communication_protocols::sendJsonDocument(const JsonDocument &doc, const Command_enum command) {
    COMM_PROTOCOL response_code = send_response_READY_TO_SEND(command);
    if(response_code!=READY_TO_RECV) return response_code;

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(command);

        CrcWriter writer;
        serializeMsgPack(doc, writer);
        response_code = sendLong(writer.hash(), GET_REMINDER_B);
        if(response_code!=SUCCESS) return response_code;

        response_code = send_response_READY_TO_SEND(command,false);
        if(response_code!=READY_TO_RECV) return response_code;
        serializeMsgPack(doc,Serial1);

        response_code=get_response(GET_REMINDER_B);
        if(response_code != RETRY) return response_code;
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return TIMEOUT;
}
JsonDocument Communication_protocols::receive_jsonDocument(const Command_enum command) {
    JsonDocument doc;
    COMM_PROTOCOL response_code = get_response(command);
    if(response_code!=READY_TO_SEND) return doc;
    send_response_READY_TO_RECV(command);

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(command);
            response_code = get_response(command,false);
            if (response_code!=RETRY) return doc;
        }
        const uint32_t crc = receive_long(command);
        if(!crc) {
            close_session(command);
            return doc;
        }

        if(get_response(command)!=READY_TO_SEND) {
            close_session(command);
            return doc;
        }
        send_response_READY_TO_RECV(command);

        if(!wait_for_response(command)) return doc;
        deserializeMsgPack(doc, Serial1);
        CrcWriter writer;
        serializeMsgPack(doc, writer);
        if(writer.hash() == crc) {
            send_status_SUCCESS(command);
            return doc;
        }
        doc.clear();
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return doc;
}
COMM_PROTOCOL Communication_protocols::sendLong(const unsigned long res_long, const Command_enum command) {
    const COMM_PROTOCOL rescode = send_response_READY_TO_SEND(command);
    if(rescode!=READY_TO_RECV) return rescode;

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(command);

        const byte *res = longToByte(res_long);
        for(int i=0;i<4;i++) Serial1.write(res[i]);
        FastCRC8 CRC8;
        Serial1.write(CRC8.smbus(res ,4));
        delete res;

        const COMM_PROTOCOL response_code =get_response(command);
        if(response_code != RETRY) return response_code;
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return TIMEOUT;
}
unsigned long Communication_protocols::receive_long(const Command_enum command) {
    COMM_PROTOCOL rescode = get_response(command);
    if(rescode!=READY_TO_SEND) return 0;
    send_response_READY_TO_RECV(command);

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(command);
            rescode = get_response(command,false);
            if (rescode!=RETRY) return 0;
        }
        byte long_[4]{};
        for(int i=0;i<4;i++) { // NOLINT(*-loop-convert)
            if(!wait_for_response(command)) return 0;
            long_[i]=Serial1.read();
        }
        if(!wait_for_response(command))return 0;
        const byte crc_val = Serial1.read();
        FastCRC8 CRC8;
        if(CRC8.smbus(long_, 4)==crc_val) {
            send_status_SUCCESS(command);
            return bytesToLong(long_);
        }
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return 0;
}
IPAddress Communication_protocols::receive_IP(const Command_enum command) {
    Serial.println("RECV_IP");
    IPAddress IP;
    COMM_PROTOCOL response_code = get_response(command);
    if(response_code!=READY_TO_SEND) {
        return IP;
    }send_response_READY_TO_RECV(command);

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(command);
            response_code = get_response(command,false);
            if (response_code!=RETRY) {
                return IP;
            }
        }

        const uint32_t crc = receive_long(command);
        if(!crc) {
            close_session(command);
            Serial.println();
            return IP;
        }

        if(get_response(command)!=READY_TO_SEND) {
            close_session(command);
            return IP;
        }
        send_response_READY_TO_RECV(command);

        if(!wait_for_response(command)) return IP;
        const byte size = Serial1.read();
        uint8_t ip_byte[size];

        for(int i=0;i<size;i++) {
            if(!wait_for_response(command)) return IP;
            ip_byte[i]= Serial1.read();
        }

        FastCRC32 CRC32;
        if(CRC32.crc32(ip_byte ,size) == crc) {
            send_status_SUCCESS(command);
            String ip_str="";
            for(int i=0;i<size;i++)
                ip_str+=static_cast<char>(ip_byte[i]);
            IP.fromString(ip_str);
            return IP;
        }
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return IP;
}

COMM_PROTOCOL Communication_protocols::send_IP(const IPAddress &IP, const Command_enum command) {
    Serial.println("SND_IP");
    COMM_PROTOCOL response_code = send_response_READY_TO_SEND(command);
    if(response_code!=READY_TO_RECV) return response_code;
    String ip;
    IP.toString()=="0.0.0.0"?
        ip = "1.1.1.1":
        ip = IP.toString();

    const auto a = ip.c_str();
    uint8_t at[strlen(a)];
    for (size_t i = 0; i < strlen(a); i++) {
        at[i] = static_cast<uint8_t>(a[i]);
    }
    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(command);

        FastCRC32 CRC32;
        response_code = sendLong(CRC32.crc32(at ,strlen(a)), command);
        if(response_code!=SUCCESS) {
            return response_code;
        }

        response_code = send_response_READY_TO_SEND(command,false);
        if(response_code!=READY_TO_RECV) {
            return response_code;
        }

        Serial1.write(strlen(a));
        for(int i=0;i<strlen(a);i++)
            Serial1.write(at[i]);

        response_code=get_response(command);
        if(response_code != RETRY) {
            return response_code;
        }
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return TIMEOUT;
}

void Communication_protocols::send_request_SYN(const Command_enum command) {
    send_header(command , SYN);
}
void Communication_protocols::send_request_RETRY(const Command_enum command) {
    send_header(command , RETRY);
}
void Communication_protocols::send_status_SUCCESS(const Command_enum command) {
    send_header(command , SUCCESS);
}
void Communication_protocols::send_status_UNKW_ERROR(const Command_enum command) {
    send_header(command , UNKW_ERR);
}
void Communication_protocols::send_status_TIMEOUT(const Command_enum command) {
    send_header(command , TIMEOUT);
}
void Communication_protocols::send_response_ACK(const Command_enum command) {
    send_header(command , ACK);
}
COMM_PROTOCOL Communication_protocols::send_response_READY_TO_SEND(const Command_enum command, const bool clear_buffer) {
    send_header(command , READY_TO_SEND);
    return get_response(command, clear_buffer);
}
void Communication_protocols::send_response_READY_TO_RECV(const Command_enum command) {
    send_header(command , READY_TO_RECV);
}
void Communication_protocols::close_session(const Command_enum command) {
    send_header(command , FIN);
}
void Communication_protocols::send_header(const Command_enum command, const byte protocol_id) {
    clear_receive_buffer();
    Serial.print("SND: ");
    printHeader((command | protocol_id));
    Serial1.write((command | protocol_id));
}
COMM_PROTOCOL Communication_protocols::send_response_SYN_ACK(const Command_enum command, const bool clear_buffer) {
    send_header(command , SYN_ACK);
    return get_response(command, clear_buffer);
}

bool Communication_protocols::wait_for_response() {
    const unsigned long time_out_start = millis();
    while(!Serial1.available())
        if(millis()-time_out_start>=time_out_)
            return false;
    return true;
}
bool Communication_protocols::wait_for_response(const Command_enum command) {
    const unsigned long time_out_start = millis();
    while(!Serial1.available())
        if(millis()-time_out_start>=time_out_) {
            send_status_TIMEOUT(command);
            return false;
        }
    return true;
}

COMM_PROTOCOL Communication_protocols::get_response(const Command_enum command, const bool clear_buffer)  {
    COMM_PROTOCOL protocol = UNKW_ERR;
    if(!wait_for_response(command)) return TIMEOUT;
    const byte response_header = Serial1.read();
    if(getCommand(response_header)==command) {
        const byte byte_protocol = getProtocol(response_header);
        if(byte_protocol==ACK) {
            protocol = ACK;
        }else if(byte_protocol==SYN_ACK) {
            protocol = SYN_ACK;
        }else if(byte_protocol==READY_TO_RECV) {
            protocol = READY_TO_RECV;
        }else if(byte_protocol==READY_TO_SEND) {
            protocol = READY_TO_SEND;
        }else if(byte_protocol==RETRY) {
            protocol = RETRY;
        }else if(byte_protocol==FIN) {
            protocol = FIN;
        }else if(byte_protocol==SUCCESS) {
            protocol = SUCCESS;
        }else if(byte_protocol==TIMEOUT) {
            protocol = TIMEOUT;
        }
    }
    if(clear_buffer) clear_receive_buffer();
    Serial.print("RCV: ");
    printHeader((command | protocol));
    return protocol;
}

void Communication_protocols::clear_receive_buffer() {
    while(Serial1.available()>0) Serial1.read();
}

void Communication_protocols::printHeader(const byte header) {
    Serial.print(command_as_String(getCommand(header)));
    Serial.print(" : ");
    Serial.print(protocol_as_String(getProtocol(header)));
    Serial.println();
}

String Communication_protocols::protocol_as_String(const byte comm_protocol) {
    if(comm_protocol==SYN) {
        return "SYN";
    }if(comm_protocol==SYN_ACK) {
        return "SYN_ACK";
    }if(comm_protocol==ACK) {
        return "ACK";
    }if(comm_protocol==READY_TO_RECV) {
        return "READY_TO_RECV";
    }if(comm_protocol==READY_TO_SEND) {
        return "READY_TO_SEND";
    }if(comm_protocol==FIN) {
        return "FIN";
    }if(comm_protocol==UNKW_ERR) {
        return "UNKW_ERR";
    }if(comm_protocol==RETRY) {
        return "RETRY";
    }if(comm_protocol==SUCCESS) {
        return "SUCCESS";
    }if(comm_protocol==TIMEOUT) {
        return "TIMEOUT";
    }
    return "unknown_protocol";
}

String Communication_protocols::command_as_String(const byte command) {
    if(command == GET_TIME) {
        return "GET_TIME";
    }if(command == GET_REMINDER_B) {
        return "GET_REMB";
    }if(command == ACTIVATE_AP) {
        return "ACT_AP";
    }if(command == DEACTIVATE_AP) {
        return "DCT_AP";
    }if(command == GET_NETWORK_INF) {
        return "GET_NETWORK_INF";
    }if(command == DAYLIGHT_SAV) {
        return "DAYLIGHT_SAV";
    }if(command == SERVER_IP) {
        return "SERVER_IP";
    }if(command == REMINDERB_CH) {
        return "REMINDERB_CH";
    }
    return "unknown_command";
}





