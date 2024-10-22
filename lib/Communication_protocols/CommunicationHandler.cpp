//
// Created by user on 22/10/2024.
//

#include "CommunicationHandler.h"

#include <Memmory.h>
#include <Network_communications.h>
#include <NTPClient.h>
#include <Output.h>
#include <WiFi.h>


bool getTimeSuccess = false;

extern Error_Codes error_codes;
extern NTPClient timeClient;

struct {
    CommandStatus status = COMPLETED;
    const unsigned long retry_interval_on_fail=5000;
    unsigned long retry_millis=0;

}deactivate_ap_status;

void CommunicationHandler::handle_communications() {
    if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)<0) {
        timeClient.update();
    }if(Serial1.available()){
        const byte response_header = Serial1.read();
        clear_receive_buffer();
        handle_header(response_header);
    }
    if(deactivate_ap_status.status==IN_PROGRESS || deactivate_ap_status.status==FAILED ) {
        if(millis()-deactivate_ap_status.retry_interval_on_fail>deactivate_ap_status.retry_millis) {
            deactivate_ap_status.retry_millis = millis();
            send_command_deactivate_ap();
        }
    }
}

void CommunicationHandler::handle_header(const byte response_header) {
    if(getCommand(response_header)==GET_TIME){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = NTP_request_handler(GET_TIME);
        }
    }else if(getCommand(response_header)==GET_REMINDER_B){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = get_reminder_b_request_handler(GET_REMINDER_B);

        }
    }else if(getCommand(response_header)==ACTIVATE_AP){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = activate_AP_request_handler(ACTIVATE_AP);

        }
    }else if(getCommand(response_header)==DEACTIVATE_AP){
        if(getProtocol(response_header)==SYN) {
            getTimeSuccess = deactivate_AP_request_handler(DEACTIVATE_AP);
        }else if(getProtocol(response_header)==SYN_ACK) {
            Serial.println("Got SYN_ACK_");
            deactivate_AP_response_handler(DEACTIVATE_AP);
        }
    }
}

bool CommunicationHandler::activate_AP_request_handler(const Commands command, const byte max_retries) {
    if(!send_response_SYN_ACK(command)==ACK) return false;
    const IPAddress IP = Network_communications::setAccessPoint();
    if(send_IP(IP,command)==SUCCESS) return true;
    close_session(command);
    return false;
}

bool CommunicationHandler::deactivate_AP_request_handler(const Commands command, const byte max_retries) {
    if(!send_response_SYN_ACK(command)==ACK) return false;
    if(WiFiClass::getMode()==WIFI_MODE_AP || WiFiClass::getMode()==WIFI_MODE_APSTA) {
        Network_communications::initializeWiFi();
    }
    send_status_SUCCESS(command);
    if(get_response(command)==SUCCESS)
        deactivate_ap_status.status=COMPLETED;
    return true;
}

void CommunicationHandler::send_command_deactivate_ap() {
    deactivate_ap_status.status=IN_PROGRESS;
    send_request_SYN(DEACTIVATE_AP);
}

bool CommunicationHandler::deactivate_AP_response_handler(const Commands command)  {
    send_response_ACK(command);
    const COMM_PROTOCOL response_code = get_response(command);
    if(response_code!=SUCCESS || response_code!= FIN) {
        deactivate_ap_status.status=FAILED;
        return false;
    }
    deactivate_ap_status.status=COMPLETED;
    return true;
}




bool CommunicationHandler::NTP_request_handler(const Commands command) {
    if (timeClient.isTimeSet()) {
        if(send_response_SYN_ACK(command)==ACK)
            if(sendTime(command)==SUCCESS)
                return true;
    }
    close_session(GET_TIME);

    return false;
}




bool CommunicationHandler::get_reminder_b_request_handler(const Commands command, const byte max_retries) {
    if(!send_response_SYN_ACK(command)==ACK) return false;
    const unsigned long forTime = getLongFromBuffer(command);
    if(!forTime) {
        send_status_UNKW_ERROR(command);
        return false;
    }

    JsonDocument doc =  Memmory::get_all_reminders_from_sd();
    doc = Memmory::get_latest_Reminder(forTime, doc);
    doc = simplify_Json(doc);
    const COMM_PROTOCOL response_code =  sendJsonDocument(doc, command, max_retries);
    if(response_code== SUCCESS) return true;
    send_status_UNKW_ERROR(command);
    return false;
}

COMM_PROTOCOL CommunicationHandler::sendTime(const Commands command) {
    unsigned long unix_time = timeClient.getEpochTime();
    if(timeClient.forceUpdate())
        unix_time = timeClient.getEpochTime();
    const COMM_PROTOCOL response_code = sendLong(unix_time, command);
    if(response_code == SUCCESS) {
        Output::print("SUCCESS: ");
        Output::println(Output::get_formated_Time(unix_time));
    }
    return response_code;
}

bool CommunicationHandler::initializeHardwareSerial(){
    Serial1.begin(115200,SERIAL_8N1,16,17);
    if (!Serial1) {
        error_codes.add_error(SOFT_SERIAL_ERROR);
        return false;
    }
    error_codes.remove_error(SOFT_SERIAL_ERROR);
    return true;
}