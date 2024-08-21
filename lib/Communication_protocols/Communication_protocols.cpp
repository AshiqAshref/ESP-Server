#include "Communication_protocols.h"

#include <NTPClient.h>
#include <SoftwareSerial.h>
#include <CRC.h>

extern SoftwareSerial hardwarePort;
extern NTPClient timeClient;

byte Communication_protocols::getProtocol(const byte b) {
    return b & PROTOCOL_FILTER;
}
byte Communication_protocols::getFunction_id(const byte response_header) const {
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

void Communication_protocols::handle_communications() const {
    timeClient.update();
    if(hardwarePort.available()){
        const byte response_header = hardwarePort.read();
        clear_receive_buffer();
        handle_header(response_header);
    }
}

bool getTimeSuccess = false;
void Communication_protocols::handle_header(const byte response_header) const {
    if(getFunction_id(response_header)==funct_id_getTime_){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = handleNTPcommand();
        }
    }
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
void Communication_protocols::send_status_TIMEOUT(const byte function_id) {
    send_header(function_id , TIMEOUT);
}
void Communication_protocols::close_session(const byte function_id) {
    send_header(function_id , FIN);
}
void Communication_protocols::send_header(const byte function_id, const byte protocol_id) {
    clear_receive_buffer();
    hardwarePort.write((function_id | protocol_id));
}

bool Communication_protocols::handleNTPcommand(const byte max_retries) const {
    if (timeClient.isTimeSet()) {
        COMM_PROTOCOL res_code = send_response_SYN_ACK(funct_id_getTime_);
        if(res_code==ACK){
            byte current_retries = 0;
            while(current_retries<max_retries) {
                res_code = sendTime();
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



COMM_PROTOCOL Communication_protocols::send_response_SYN_ACK(const byte function_id) const {
    send_header(function_id , SYN_ACK);
    return get_response(function_id);
}

COMM_PROTOCOL Communication_protocols::sendTime() const {
    unsigned long l_res = timeClient.getEpochTime();
    if(timeClient.forceUpdate())
        l_res = timeClient.getEpochTime();
    Serial.print("time here: ");
    Serial.println(l_res);
    sendLong(l_res);
    return get_response(funct_id_getTime_);
}

void Communication_protocols::sendLong(const unsigned long res_long) {
    const byte *res = longToByte(res_long);
    for(int i=0;i<4;i++)
        hardwarePort.write(res[i]);
    hardwarePort.write(calcCRC8(res,4));
    delete res;
}

bool Communication_protocols::wait_for_response() const {
    const unsigned long time_out_start = millis();
    while(!hardwarePort.available())
        if(millis()-time_out_start>=time_out_)
            return false;
    return true;
}

COMM_PROTOCOL Communication_protocols::get_response(const byte function_id) const {
    if(!wait_for_response())
        return TIMEOUT;
    while(hardwarePort.available()) {
        const byte response_header = hardwarePort.read();
        if(getFunction_id(response_header)==function_id) {
            if(getProtocol(response_header)==ACK) {
                return ACK;
            }if(getProtocol(response_header)==RETRY) {
                return RETRY;
            }if(getProtocol(response_header)==FIN) {
                return FIN;
            }if(getProtocol(response_header)==SUCCESS) {
                return SUCCESS;
            }
        }
        clear_receive_buffer();
    }
    return UNKW_ERR;
}

void Communication_protocols::clear_receive_buffer() {
    while(hardwarePort.available()>0) hardwarePort.read();
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
