#include "Communication_protocols.h"

#include <NTPClient.h>
#include <CrcWriter.h>
#include <Memmory.h>
#include <FastCRC.h>


// extern Output output;

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

COMM_PROTOCOL Communication_protocols::sendJsonDocument(const JsonDocument &doc, const Commands command, const byte max_retries) {
    COMM_PROTOCOL response_code = send_response_SYN_ACK(command);
    if(response_code!=ACK) return response_code;

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(command);

        CrcWriter writer;
        serializeMsgPack(doc, writer);
        response_code = sendLong(writer.hash(), GET_REMINDER_B);
        if(response_code!=SUCCESS) return response_code;

        response_code = send_response_SYN_ACK(command,false);
        if(response_code!=ACK) return response_code;
        serializeMsgPack(doc,Serial1);

        response_code=get_response(GET_REMINDER_B);
        if(response_code != RETRY) return response_code;
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return TIMEOUT;
}
JsonDocument Communication_protocols::receive_jsonDocument(const Commands command, const byte max_retries) {
    JsonDocument doc;
    COMM_PROTOCOL response_code = get_response(command);
    if(response_code!=SYN_ACK) return doc;
    send_response_ACK(command);

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(command);
            response_code = get_response(command,false);
            if (response_code!=RETRY) return doc;
        }
        const uint32_t crc = getLongFromBuffer(command);
        if(!crc) {
            close_session(command);
            return doc;
        }

        if(get_response(command)!=SYN_ACK) {
            close_session(command);
            return doc;
        }
        send_response_ACK(command);

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
COMM_PROTOCOL Communication_protocols::sendLong(const unsigned long res_long, const Commands command) {
    const COMM_PROTOCOL rescode = send_response_SYN_ACK(command);
    if(rescode!=ACK) return rescode;

    constexpr byte max_retries = 20;
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
unsigned long Communication_protocols::getLongFromBuffer(const Commands command) {
    COMM_PROTOCOL rescode = get_response(command);
    if(rescode!=SYN_ACK) return 0;
    send_response_ACK(command);

    constexpr byte max_retries = 20;
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
IPAddress Communication_protocols::receive_IP(const Commands command, const byte max_retries) {
    IPAddress IP;
    COMM_PROTOCOL response_code = get_response(command);
    if(response_code!=SYN_ACK) return IP;
    send_response_ACK(command);

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(command);
            response_code = get_response(command,false);
            if (response_code!=RETRY) return IP;
        }
        const uint32_t crc = getLongFromBuffer(command);
        if(!crc) {
            close_session(command);
            return IP;
        }

        if(get_response(command)!=SYN_ACK) {
            close_session(command);
            return IP;
        }
        send_response_ACK(command);

        if(!wait_for_response(command)) return IP;
        const byte size = Serial1.read();
        uint8_t ip_byte[size];
        for(int i=0;i<size;i++)
            ip_byte[size]= Serial1.read();

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
COMM_PROTOCOL Communication_protocols::send_IP(const IPAddress &IP, const Commands command) {
    COMM_PROTOCOL response_code = send_response_SYN_ACK(command);
    if(response_code!=ACK) return response_code;

    const String ip = IP.toString();
    const auto a = ip.c_str();
    uint8_t at[strlen(a)];
    for (size_t i = 0; i < strlen(a); i++) {
        at[i] = static_cast<uint8_t>(a[i]);
    }
    constexpr byte max_retries= 10;
    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(command);

        FastCRC32 CRC32;
        response_code = sendLong(CRC32.crc32(at ,strlen(a)), command);
        if(response_code!=SUCCESS) return response_code;

        response_code = send_response_SYN_ACK(command,false);
        if(response_code!=ACK) return response_code;
        Serial1.write(strlen(a));
        for(int i=0;i<strlen(a);i++)
            Serial1.write(at[i]);

        response_code=get_response(command);
        if(response_code != RETRY) return response_code;
        current_retries++;
    }
    send_status_TIMEOUT(command);
    return TIMEOUT;
}

void Communication_protocols::send_request_SYN(const Commands command) {
    send_header(command , SYN);
}
void Communication_protocols::send_request_RETRY(const Commands command) {
    send_header(command , RETRY);
}
void Communication_protocols::send_status_SUCCESS(const Commands command) {
    send_header(command , SUCCESS);
}
void Communication_protocols::send_status_UNKW_ERROR(const Commands command) {
    send_header(command , UNKW_ERR);
}
void Communication_protocols::send_status_TIMEOUT(const Commands command) {
    send_header(command , TIMEOUT);
}
void Communication_protocols::send_response_ACK(const Commands command) {
    send_header(command , ACK);
}
void Communication_protocols::close_session(const Commands command) {
    send_header(command , FIN);
}
void Communication_protocols::send_header(const byte command, const byte protocol_id) {
    clear_receive_buffer();
    Serial1.write((command | protocol_id));
}
COMM_PROTOCOL Communication_protocols::send_response_SYN_ACK(const Commands command, const bool clear_buffer) {
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
bool Communication_protocols::wait_for_response(const Commands command) {
    const unsigned long time_out_start = millis();
    while(!Serial1.available())
        if(millis()-time_out_start>=time_out_) {
            send_status_TIMEOUT(command);
            return false;
        }
    return true;
}

COMM_PROTOCOL Communication_protocols::get_response(const Commands command, const bool clear_buffer)  {
    if(!wait_for_response())
        return TIMEOUT;
    while(Serial1.available()) {
        const byte response_header = Serial1.read();
        if(getCommand(response_header)==command) {
            if(getProtocol(response_header)==ACK) {
                return ACK;
            }if(getProtocol(response_header)==SYN_ACK) {
                return SYN_ACK;
            }if(getProtocol(response_header)==RETRY) {
                return RETRY;
            }if(getProtocol(response_header)==FIN) {
                return FIN;
            }if(getProtocol(response_header)==SUCCESS) {
                return SUCCESS;
            }
        }
        if(clear_buffer) clear_receive_buffer();
    }
    return UNKW_ERR;
}


void Communication_protocols::clear_receive_buffer() {
    while(Serial1.available()>0) Serial1.read();
}

JsonDocument Communication_protocols::simplify_Json(JsonDocument doc){
    JsonDocument main_doc;
    auto main_doc_array= main_doc.to<JsonArray>();
    size_t doc_array_size = doc.size();
    for(size_t i=0; i<doc_array_size; i++){
        JsonDocument reminder_doc;
        reminder_doc["ti"] =doc[i]["timeId"];
        reminder_doc["t"] = doc[i]["time"].as<String>();
        auto med_array = reminder_doc["m"].to<JsonArray>();
        size_t med_array_size = doc[i]["medicines"].size();
        for(size_t j=0; j<med_array_size; j++){
            JsonDocument med_doc;
            med_doc["b"] = doc[i]["medicines"][j]["medBox"];
            med_doc["d"] = doc[i]["medicines"][j]["dosage"];
            med_doc["s"] = doc[i]["medicines"][j]["success"]?1:0;
            // ReSharper disable once CppExpressionWithoutSideEffects
            med_array.add(med_doc);
        }
        // ReSharper disable once CppExpressionWithoutSideEffects
        main_doc_array.add(reminder_doc);
    }
    doc.clear();
    return main_doc;
}
JsonDocument Communication_protocols::unsimplify_Json(JsonDocument doc){
    JsonDocument main_doc;
    auto main_doc_array= main_doc.to<JsonArray>();
    size_t doc_array_size = doc.size();
    for(size_t i=0; i<doc_array_size; i++){
        JsonDocument reminder_doc;
        reminder_doc["timeId"] =doc[i]["ti"];
        reminder_doc["time"] = doc[i]["t"].as<String>();
        auto med_array = reminder_doc["medicines"].to<JsonArray>();
        size_t med_array_size = doc[i]["m"].size();
        for(size_t j=0; j<med_array_size; j++){
            JsonDocument med_doc;
            med_doc["medBox"] = doc[i]["m"][j]["b"];
            med_doc["dosage"] = doc[i]["m"][j]["d"];
            med_doc["success"] = doc[i]["m"][j]["s"]?true:false;
            // ReSharper disable once CppExpressionWithoutSideEffects
            med_array.add(med_doc);
        }
        // ReSharper disable once CppExpressionWithoutSideEffects
        main_doc_array.add(reminder_doc);
    }
    doc.clear();
    return main_doc;
}





