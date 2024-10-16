#include "Communication_protocols.h"

#include <NTPClient.h>
#include <Error_Codes.h>
#include <Output.h>
#include <CrcWriter.h>
#include <Memmory.h>
#include <FastCRC.h>

extern NTPClient timeClient;
extern Error_Codes error_codes;
extern Memmory memmory;

// extern Output output;

byte Communication_protocols::getProtocol(const byte b) {
    return b & PROTOCOL_FILTER;
}
byte Communication_protocols::getFunction_id(const byte response_header)  {
    return response_header & function_id_filter_;
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

void Communication_protocols::handle_communications() {
    if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)<0) {
        timeClient.update();
    }if(Serial1.available()){
        const byte response_header = Serial1.read();
        clear_receive_buffer();
        handle_header(response_header);
    }
}

bool getTimeSuccess = false;
void Communication_protocols::handle_header(const byte response_header) {
    if(getFunction_id(response_header)==funct_id_getTime_){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = NTP_request_handler(funct_id_getTime_);
        }
    }else if(getFunction_id(response_header)==funct_id_get_reminderB_){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = get_reminder_b_request_handler(funct_id_get_reminderB_);
        }
    }
}

bool Communication_protocols::NTP_request_handler(const byte function_id, const byte max_retries) {
    if (timeClient.isTimeSet()) {
        COMM_PROTOCOL res_code = send_response_SYN_ACK(function_id);
        if(res_code==ACK){
            byte current_retries = 0;
            while(current_retries<max_retries) {
                res_code = sendTime(function_id);
                if(res_code==RETRY) {
                    current_retries++;
                }else if(res_code==SUCCESS) {
                    return true;
                }
            }
        }
    }
    close_session(funct_id_getTime_);
    return false;
}


bool Communication_protocols::get_reminder_b_request_handler(const byte function_id, const byte max_retries) {
    if(!send_response_SYN_ACK(function_id)==ACK) return false;
    const unsigned long forTime = getLongFromBuffer(function_id);
    if(!forTime) {
        close_session(function_id);
        return false;
    }

    JsonDocument doc =  memmory.get_all_reminders_from_sd();
    doc = Memmory::get_latest_Reminder(forTime, doc);
    doc = simplify_Json(doc);
    const COMM_PROTOCOL response_code =  sendJsonDocument(doc, max_retries);
    if(response_code== SUCCESS) return true;
    send_status_UNKW_ERROR(function_id);
    return false;
}



COMM_PROTOCOL Communication_protocols::sendJsonDocument(const JsonDocument &doc, const byte function_id, const byte max_retries) {
    COMM_PROTOCOL response_code = send_response_SYN_ACK(function_id);
    if(response_code!=ACK) return response_code;

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(function_id);

        CrcWriter writer;
        serializeMsgPack(doc, writer);
        response_code = sendLong(writer.hash(), funct_id_get_reminderB_);
        if(response_code!=SUCCESS) return response_code;

        response_code = send_response_SYN_ACK(function_id,false);
        if(response_code!=ACK) return response_code;
        serializeMsgPack(doc,Serial1);

        response_code=get_response(funct_id_get_reminderB_);
        if(response_code != RETRY) return response_code;
        current_retries++;
    }
    send_status_TIMEOUT(function_id);
    return TIMEOUT;
}

JsonDocument Communication_protocols::receive_jsonDocument(const byte function_id, const byte max_retries) {
    JsonDocument doc;
    COMM_PROTOCOL response_code = get_response(function_id);
    if(response_code!=SYN_ACK) return doc;
    send_response_ACK(function_id);

    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(function_id);
            response_code = get_response(function_id,false);
            if (response_code!=RETRY) return doc;
        }
        const uint32_t crc = getLongFromBuffer(function_id);
        if(!crc) {
            close_session(function_id);
            return doc;
        }

        if(get_response(function_id)!=SYN_ACK) {
            close_session(function_id);
            return doc;
        }
        send_response_ACK(function_id);

        if(!wait_for_response(function_id)) return doc;
        deserializeMsgPack(doc, Serial1);
        CrcWriter writer;
        serializeMsgPack(doc, writer);
        if(writer.hash() == crc) {
            send_status_SUCCESS(function_id);
            return doc;
        }
        doc.clear();
        current_retries++;
    }
    send_status_TIMEOUT(function_id);
    return doc;
}

COMM_PROTOCOL Communication_protocols::sendLong(const unsigned long res_long, const byte function_id) {
    const COMM_PROTOCOL rescode = send_response_SYN_ACK(function_id);
    if(rescode!=ACK) return rescode;

    constexpr byte max_retries = 20;
    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) send_request_RETRY(function_id);

        const byte *res = longToByte(res_long);
        for(int i=0;i<4;i++) Serial1.write(res[i]);
        FastCRC8 CRC8;
        Serial1.write(CRC8.smbus(res ,4));
        delete res;

        const COMM_PROTOCOL response_code =get_response(function_id);
        if(response_code != RETRY) return response_code;
        current_retries++;
    }
    send_status_TIMEOUT(function_id);
    return TIMEOUT;
}

unsigned long Communication_protocols::getLongFromBuffer(const byte function_id) {
    COMM_PROTOCOL rescode = get_response(function_id);
    if(rescode!=SYN_ACK) return 0;
    send_response_ACK(function_id);

    constexpr byte max_retries = 20;
    byte current_retries = 0;
    while(current_retries<max_retries) {
        if(current_retries>0) {
            send_request_RETRY(function_id);
            rescode = get_response(function_id,false);
            if (rescode!=RETRY) return 0;
        }
        byte long_[4]{};
        for(int i=0;i<4;i++) { // NOLINT(*-loop-convert)
            if(!wait_for_response(function_id)) return 0;
            long_[i]=Serial1.read();
        }
        if(!wait_for_response(function_id))return 0;
        const byte crc_val = Serial1.read();
        FastCRC8 CRC8;
        if(CRC8.smbus(long_, 4)==crc_val) {
            send_status_SUCCESS(function_id);
            return bytesToLong(long_);
        }
        current_retries++;
    }
    send_status_TIMEOUT(function_id);
    return 0;
}




void Communication_protocols::send_request_SYN(const byte function_id) {
    send_header(function_id , SYN);
}
void Communication_protocols::send_request_RETRY(const byte function_id) {
    send_header(function_id , RETRY);
}
void Communication_protocols::send_status_SUCCESS(const byte function_id) {
    send_header(function_id , SUCCESS);
}
void Communication_protocols::send_status_UNKW_ERROR(const byte function_id) {
    send_header(function_id , UNKW_ERR);
}
void Communication_protocols::send_status_TIMEOUT(const byte function_id) {
    send_header(function_id , TIMEOUT);
}
void Communication_protocols::send_response_ACK(const byte function_id) {
    send_header(function_id , ACK);
}
void Communication_protocols::close_session(const byte function_id) {
    send_header(function_id , FIN);
}
void Communication_protocols::send_header(const byte function_id, const byte protocol_id) {
    clear_receive_buffer();
    Serial1.write((function_id | protocol_id));
}


COMM_PROTOCOL Communication_protocols::send_response_SYN_ACK(const byte function_id, const bool clear_buffer) {
    send_header(function_id , SYN_ACK);
    return get_response(function_id, clear_buffer);
}

COMM_PROTOCOL Communication_protocols::sendTime(const byte function_id) {
    unsigned long unix_time = timeClient.getEpochTime();
    if(timeClient.forceUpdate())
        unix_time = timeClient.getEpochTime();
    const COMM_PROTOCOL response_code = sendLong(unix_time, function_id);
    if(response_code == SUCCESS) {
        Output::print("syn: ");
        Output::println(Output::get_formated_Time(unix_time));
    }
    return response_code;
}




void Communication_protocols::printBin(const byte aByte) {
    Serial.print('(');
    for (int8_t aBit = 7; aBit >= 0; aBit--) {
        Serial.print(bitRead(aByte, aBit) ? '1' : '0');
        if(aBit==4) Serial.print(' ');
    }
    Serial.print(')');
}
void Communication_protocols::printlnBin(const byte aByte) {
    printBin(aByte);
    Serial.println();
}



bool Communication_protocols::wait_for_response() {
    const unsigned long time_out_start = millis();
    while(!Serial1.available())
        if(millis()-time_out_start>=time_out_)
            return false;
    return true;
}

bool Communication_protocols::wait_for_response(const byte function_id) {
    const unsigned long time_out_start = millis();
    while(!Serial1.available())
        if(millis()-time_out_start>=time_out_) {
            send_status_TIMEOUT(function_id);
            return false;
        }
    return true;
}

COMM_PROTOCOL Communication_protocols::get_response(const byte function_id, const bool clear_buffer)  {
    if(!wait_for_response())
        return TIMEOUT;
    while(Serial1.available()) {
        const byte response_header = Serial1.read();
        if(getFunction_id(response_header)==function_id) {
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