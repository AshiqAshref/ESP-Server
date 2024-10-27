#include "CommunicationHandler.h"

#include <AV_Functions.h>
#include <Command_activate_AP.h>
#include <Command_deactivate_ap.h>
#include <Command_get_reminderB.h>
#include <Command_get_time.h>

#include <Memmory.h>
#include <Network_communications.h>
#include <NTPClient.h>
#include <Output.h>
#include <WiFi.h>

bool getTimeSuccess = false;

extern Error_Codes error_codes;
extern NTPClient timeClient;

extern Command_get_time command_get_time;
extern Command_get_reminderB command_get_reminder_b;
extern Command_activate_AP command_activate_AP;
extern Command_deactivate_ap command_deactivate_ap;
constexpr byte max_retries=20;

constexpr byte commands_size=4;
Command *commands[commands_size]= { &command_get_reminder_b, &command_activate_AP,&command_deactivate_ap, &command_get_time,}; // NOLINT(*-slicing)


void CommunicationHandler::handle_communications() {
    if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)<0) {
        timeClient.update();
    }
    if(Serial1.available()){
        const byte response_header = Serial1.read();
        Serial.print("RCV: ");
        printHeader(response_header);
        clear_receive_buffer();
        handle_header(response_header);
    }

    for(int i=0;i<commands_size;i++) {
        if(commands[i]->status()==IN_PROGRESS || commands[i]->status()==FAILED) {
            if(millis()-commands[i]->last_millis>commands[i]->retry_interval) {
                commands[i]->send_request();
            }
        }
    }
}

void CommunicationHandler::handle_header(const byte response_header) {
    Serial.print("checking for ");
    printHeader(response_header);
    for(int i=0;i<commands_size;i++) {
        Serial.print("checking for ");
        Serial.println(commands[i]->command());
        AV_Functions::printlnBin(commands[i]->command());

        if(getCommand(response_header)==commands[i]->command()) {
            Serial.print("handling: ");
            if(getProtocol(response_header)==SYN_ACK) {
                printHeader(response_header);
                commands[i]->response_handler();
                break;
            }if(getProtocol(response_header)==SYN) {
                printHeader(response_header);
                commands[i]->request_handler();
                break;
            }if(getProtocol(response_header)==TIMEOUT) {
                printHeader(response_header);
                commands[i]->set_status(FAILED);
                break;
            }if(getProtocol(response_header)==UNKW_ERR) {
                printHeader(response_header);
                commands[i]->set_status(FAILED);
                break;
            }if(getProtocol(response_header)==SUCCESS) {
                printHeader(response_header);
                commands[i]->set_status(COMPLETED);
                break;
            }if(getProtocol(response_header)==FIN) {
                printHeader(response_header);
                commands[i]->set_status(FAILED);
                break;
            }
            Serial.println("FOUND NADA");
            send_status_UNKW_ERROR(commands[i]->command());
            break;
        }
    }
    Serial.println();
    clear_receive_buffer();
}

void CommunicationHandler::printmode() {
    wifi_mode_t mode = WiFiClass::getMode();
    Output::print("mode: ");
    if(mode == WIFI_MODE_AP) {
        Output::print("AP",false);
    }else if(mode == WIFI_MODE_APSTA) {
        Output::print("APSTA",false);
    }else if(mode == WIFI_MODE_MAX) {
        Output::print("MAX",false);
    }else if(mode == WIFI_MODE_STA) {
        Output::print("STA",false);
    }else if(mode == WIFI_MODE_NULL) {
        Output::print("NULL",false);
    }
}

bool CommunicationHandler::activate_AP_request_handler() {
    Serial.println("activate ap REQ_H");
    constexpr Command_enum command = ACTIVATE_AP;
    if(send_response_SYN_ACK(command)!=ACK) return false;

    if(WiFiClass::getMode()==WIFI_MODE_STA || WiFiClass::getMode()==WIFI_MODE_NULL) {
        if(send_response_SYN_ACK(command)!=ACK) return false;
        const IPAddress IP = Network_communications::setAccessPoint();
        if(send_IP(IP,command)==SUCCESS) return true;
    }else if(WiFiClass::getMode()==WIFI_MODE_AP || WiFiClass::getMode()==WIFI_MODE_APSTA) {
        if(send_response_SYN_ACK(command)!=ACK) return false;
        const IPAddress IP = Network_communications::getAPIP();
        if(send_IP(IP,command)==SUCCESS) return true;
    }
    close_session(command);
    return true;
}
void CommunicationHandler::send_command_deactivate_ap() {
    Serial.println("SND DCT_AP");
    send_request_SYN(DEACTIVATE_AP);
}
bool CommunicationHandler::deactivate_AP_response_handler()  {
    Serial.println("deactivate ap RES_H");
    constexpr Command_enum command = DEACTIVATE_AP;
    send_response_ACK(command);
    const COMM_PROTOCOL response_code = get_response(command);
    if(response_code==SUCCESS || response_code== FIN)
        return true;
    return false;
}
bool CommunicationHandler::deactivate_AP_request_handler() {
    Serial.println("deactivate ap REQ_H");
    constexpr Command_enum command = DEACTIVATE_AP;
    if(send_response_SYN_ACK(command)!=ACK) return false;
    if(WiFiClass::getMode()==WIFI_MODE_AP || WiFiClass::getMode()==WIFI_MODE_APSTA) {
        Network_communications::initializeWiFi();
    }
    send_status_SUCCESS(command);
    if(get_response(command)==SUCCESS)
        return true;
    return false;
}

bool CommunicationHandler::NTP_request_handler() {
    Serial.println("NTP REQ_H");
    constexpr Command_enum command = GET_TIME;
    if (timeClient.isTimeSet()) {
        if(send_response_SYN_ACK(command)==ACK)
            if(sendTime()==SUCCESS)
                return true;
    }
    close_session(command);
    return false;
}

COMM_PROTOCOL CommunicationHandler::sendTime() {
    constexpr Command_enum command = GET_TIME;
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

bool CommunicationHandler::reminder_b_request_handler() {
    Serial.println("REMB REQ_H");
    constexpr Command_enum command = GET_REMINDER_B;
    if(send_response_SYN_ACK(command)!=ACK) return false;
    const unsigned long forTime = getLongFromBuffer(command);
    if(!forTime) {
        send_status_UNKW_ERROR(command);
        return false;
    }
    JsonDocument doc =  Memmory::get_all_reminders_from_sd();
    doc = Memmory::get_latest_Reminder(forTime, doc);
    const COMM_PROTOCOL response_code =  sendJsonDocument(doc, command);
    if(response_code== SUCCESS) return true;
    send_status_UNKW_ERROR(command);
    return false;
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





