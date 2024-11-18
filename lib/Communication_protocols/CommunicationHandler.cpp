#include "CommunicationHandler.h"

#include <AV_Functions.h>
#include <Command_activate_AP.h>
#include <Command_daylight_sav.h>
#include <Command_deactivate_ap.h>
#include <Command_get_network_inf.h>
#include <Command_get_reminderB.h>
#include <Command_get_time.h>
#include <Command_server_ip.h>
#include <Command_reminderB_change.h>
#include <Command_reminderB_send_log.h>


#include <Memmory.h>
#include <Network_communications.h>
#include <Net_resource_post_remB_stat.h>
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
extern Command_get_network_inf command_get_network_inf;
extern Command_daylight_sav command_daylight_sav;
extern Command_server_ip command_server_ip;
extern Command_reminderB_change command_reminderB_change;
extern Command_reminderB_send_log command_reminderB_send_log;

constexpr byte max_retries=20;

constexpr byte commands_size=9;
Command *commands[commands_size]= {
    &command_get_reminder_b,
    &command_activate_AP,
    &command_deactivate_ap,
    &command_get_time,
    &command_get_network_inf,
    &command_daylight_sav,
    &command_server_ip,
    &command_reminderB_change,
    &command_reminderB_send_log
}; // NOLINT(*-slicing)


void CommunicationHandler::handle_communications() {
    if(Serial1.available()){
        const byte response_header = Serial1.read();
        Serial.println();
        Serial.print("RCV_MAIN: ");
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
    for(int i=0;i<commands_size;i++) {
        if(getCommand(response_header)==commands[i]->command()) {
            if(getProtocol(response_header)==SYN_ACK) {
                commands[i]->response_handler();
                break;
            }if(getProtocol(response_header)==SYN) {
                commands[i]->request_handler();
                break;
            }if(getProtocol(response_header)==TIMEOUT) {
                commands[i]->set_status(FAILED);
                break;
            }if(getProtocol(response_header)==UNKW_ERR) {
                commands[i]->set_status(FAILED);
                break;
            }if(getProtocol(response_header)==SUCCESS) {
                commands[i]->set_status(COMPLETED);
                break;
            }if(getProtocol(response_header)==FIN) {
                commands[i]->set_status(FAILED);
                break;
            }
            Serial.print("FOUND NADA: ");
            AV_Functions::printlnBin(response_header);
            send_status_UNKW_ERROR(commands[i]->command());
            break;
        }
    }
    clear_receive_buffer();
}

void CommunicationHandler::printmode(const wifi_mode_t mode) {
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
void CommunicationHandler::printmode() {
    const wifi_mode_t mode = WiFiClass::getMode();
    printmode(mode);
}

bool CommunicationHandler::activate_AP_request_handler() {
    Serial.println("ACT_AP - REQ_H");
    constexpr Command_enum command = ACTIVATE_AP;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}

    if(WiFiClass::getMode()==WIFI_MODE_STA || WiFiClass::getMode()==WIFI_MODE_NULL) {
        if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
        const IPAddress IP = Network_communications::setAccessPoint();
        if(send_IP(IP,command)==SUCCESS) return true;
    }else if(WiFiClass::getMode()==WIFI_MODE_AP || WiFiClass::getMode()==WIFI_MODE_APSTA) {
        if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
        const IPAddress IP = Network_communications::getAPIP();
        if(send_IP(IP,command)==SUCCESS) return true;
    }
    close_session(command);
    return true;
}
void CommunicationHandler::send_command_deactivate_ap() {
    send_request_SYN(DEACTIVATE_AP);
}
bool CommunicationHandler::deactivate_AP_response_handler()  {
    Serial.println("DCT-AP - RES_H");
    constexpr Command_enum command = DEACTIVATE_AP;
    send_response_ACK(command);
    const COMM_PROTOCOL response_code = get_response(command);
    if(response_code==SUCCESS || response_code== FIN)
        return true;
    send_status_UNKW_ERROR(command);
    return false;
}
bool CommunicationHandler::deactivate_AP_request_handler() {
    Serial.println("DCT_AP REQ_H");
    constexpr Command_enum command = DEACTIVATE_AP;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}
    if(WiFiClass::getMode()==WIFI_MODE_AP || WiFiClass::getMode()==WIFI_MODE_APSTA) {
        Network_communications::initializeWiFi();
    }
    send_status_SUCCESS(command);
    if(get_response(command)==SUCCESS)
        return true;
    close_session(command);
    return false;
}

bool CommunicationHandler::NTP_request_handler() {
    Serial.println("NTP - REQ_H");
    constexpr Command_enum command = GET_TIME;
    if (timeClient.isTimeSet()) {
        if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}
        if(sendTime()!=SUCCESS) {close_session(command); return false;}
        return true;
    }
    close_session(command);
    return false;
}
COMM_PROTOCOL CommunicationHandler::sendTime() {
    constexpr Command_enum command = GET_TIME;
    unsigned long unix_time = timeClient.getEpochTime();
    const bool dts = Memmory::get_daylight_saving();
    if(timeClient.forceUpdate())
        unix_time = timeClient.getEpochTime();
    COMM_PROTOCOL response_code;
    dts?
        response_code = sendLong(unix_time+3600, command):
        response_code = sendLong(unix_time, command);

    if(response_code == SUCCESS) {
        Output::print("SUCCESS: ");
        Output::println(Output::get_formated_Time(unix_time));
    }
    return response_code;
}

bool CommunicationHandler::reminder_b_request_handler() {
    Serial.println("REMB - REQ_H");
    constexpr Command_enum command = GET_REMINDER_B;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}
    const unsigned long forTime = receive_long(command);
    if(!forTime) {
        send_status_UNKW_ERROR(command);
        return false;
    }
    JsonDocument doc = Memmory::get_latest_Reminder(forTime);
    doc["rv"]=Memmory::get_reminder_b_revision_no();
    const COMM_PROTOCOL response_code =  sendJsonDocument(doc, command);
    if(response_code== SUCCESS) return true;
    send_status_UNKW_ERROR(command);
    return false;
}

void CommunicationHandler::send_command_daylight_sav() {
    send_request_SYN(DAYLIGHT_SAV);
}
bool CommunicationHandler::daylight_sav_response_handler(const bool daylight_sav,const bool command_type) {
    Serial.println("DLT_SV RES_H");
    constexpr Command_enum command=DAYLIGHT_SAV;
    send_response_ACK(command);

    if(command_type) {
        send_response_READY_TO_RECV(command);
        if(get_response(command)!=READY_TO_SEND){close_session(command); return false;}
        send_response_READY_TO_RECV(command);
        return daylight_sav_receive_dls();
    }
    if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
    return daylight_sav_send_dls(daylight_sav);
}
bool CommunicationHandler::daylight_sav_request_handler() {
    Serial.println("DLT_SV REQ_H");
    constexpr Command_enum command = DAYLIGHT_SAV;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}

    const COMM_PROTOCOL response_code = get_response(command);
    if(response_code==READY_TO_SEND) {
        send_response_READY_TO_RECV(command);
        return daylight_sav_receive_dls();
    }
    if(response_code==READY_TO_RECV) {
        if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
        return daylight_sav_send_dls(Memmory::get_daylight_saving());
    }
    return true;
}
bool CommunicationHandler::daylight_sav_receive_dls() {
    Serial.println("DLS_SV - REC");
    constexpr Command_enum command = DAYLIGHT_SAV;
    if(get_response(command)!=READY_TO_SEND) {close_session(command); return false;}
    send_response_READY_TO_RECV(command);

    if(!wait_for_response(command)) return false;
    const byte response = Serial1.read();
    if(response==153) {
        Memmory::save_daylight_saving(true);
    }else if(response==102) {
        Memmory::save_daylight_saving(false);
    }else {
        close_session(command);
        return false;
    }
    send_status_SUCCESS(command);
    Serial.print("RCV DLS SUCC VAL: ");
    Serial.println(response);
    return true;
}
bool CommunicationHandler::daylight_sav_send_dls(const bool daylight_sav) {
    Serial.println("DLS-SV - SND");
    constexpr Command_enum command = DAYLIGHT_SAV;
    if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
    daylight_sav?
        Serial1.write(153):
        Serial1.write(102);
    Memmory::save_daylight_saving(daylight_sav);
    if(get_response(command)!=SUCCESS) {close_session(command); return false;}
    Serial.print("SENT DLS SUCC VAL: ");
    Serial.println(daylight_sav);
    return true;
}


void CommunicationHandler::send_command_server_ip() {
    send_request_SYN(SERVER_IP);
}
bool CommunicationHandler::server_ip_response_handler(const IPAddress &server_ip, const bool command_type) {
    Serial.println("SRV_IP RES_H");
    constexpr Command_enum command=SERVER_IP;
    send_response_ACK(command);

    if(command_type) {
        send_response_READY_TO_RECV(command);
        if(get_response(command)!=READY_TO_SEND){close_session(command); return false;}
        send_response_READY_TO_RECV(command);
        return server_ip_receive_ip();
    }
    if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
    return server_ip_send_ip(server_ip);
}
bool CommunicationHandler::server_ip_request_handler() {
    Serial.println("SRV_IP REQ_H");
    constexpr Command_enum command = SERVER_IP;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}

    const COMM_PROTOCOL response_code = get_response(command);
    if(response_code==READY_TO_SEND) {
        send_response_READY_TO_RECV(command);
        return server_ip_receive_ip();
    }
    if(response_code==READY_TO_RECV) {
        if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
        return server_ip_send_ip(Memmory::get_server_ip());
    }
    return true;
}
bool CommunicationHandler::server_ip_receive_ip() {
    Serial.println("SRV_IP - REC");
    constexpr Command_enum command = SERVER_IP;
    if(get_response(command)!=READY_TO_SEND) {close_session(command); return false;}
    send_response_READY_TO_RECV(command);

    const IPAddress ip =receive_IP(command);
    if(ip.toString()=="0.0.0.0") {
        close_session(command);
        return false;
    }
    Memmory::save_server_ip(ip);
    command_server_ip.set_server_ip(ip);

    send_status_SUCCESS(command);
    Serial.print("RCV SRV_IP SUCC VAL: ");
    Serial.println(ip);
    return true;
}
bool CommunicationHandler::server_ip_send_ip(const IPAddress &server_ip) {
    Serial.println("SRV_IP - SND");
    constexpr Command_enum command = SERVER_IP;
    Memmory::save_server_ip(server_ip);
    if(send_response_READY_TO_SEND(command)!=READY_TO_RECV) {close_session(command); return false;}
    if(send_IP(server_ip,command)!=SUCCESS) {close_session(command); return false;}
    Serial.print("SENT SRV_IP SUCC VAL: ");
    Serial.println(server_ip);
    return true;
}


void CommunicationHandler::send_command_get_network_inf() {
    send_request_SYN(GET_NETWORK_INF);
}
bool CommunicationHandler::get_network_inf_request_handler() {
    Serial.println("NET_INF - REQ_H");
    constexpr Command_enum command = GET_NETWORK_INF;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}
    if(WiFiClass::getMode()==WIFI_MODE_AP || WiFiClass::getMode()==WIFI_MODE_APSTA) {
        Serial1.write(24);
        Serial1.write(24+10);
        if(get_response(command)!=SUCCESS) {close_session(command); return false;}
        const IPAddress ip = Network_communications::getAPIP();
        send_IP(ip,command);
    }if(WiFiClass::getMode()==WIFI_MODE_STA ) {
        Serial1.write(60);
        Serial1.write(60+10);
        if(get_response(command)!=SUCCESS) {close_session(command); return false;}
        auto ip=IPAddress(1,1,1,1);
        if(Network_communications::wifiConnected())
            ip = Network_communications::getWifiIP();
        send_IP(ip,command);
    }else {
        printmode(WiFiClass::getMode());
        Serial1.write(36);
        Serial1.write(36+10);
        return true;
    }
    return true;
}
bool CommunicationHandler::get_network_inf_response_handler() {
    constexpr Command_enum command = GET_NETWORK_INF;
    send_response_ACK(command);
    if(get_response(command)==SUCCESS)return true;
    close_session(command);
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

bool CommunicationHandler::reminderB_change_request_handler() {
    Serial.println("RMB_CNG - REQ_H");
    constexpr Command_enum command = REMINDERB_CH;
    if(send_response_SYN_ACK(command)!=ACK) {close_session(command); return false;}

    sendLong(Memmory::get_reminder_b_revision_no(),command);
    if(get_response(command,false)!=SUCCESS){close_session(command); return false;}
    return true;
}

void CommunicationHandler::send_command_reminderB_change() {
    send_request_SYN(REMINDERB_CH);
}

bool CommunicationHandler::reminderB_change_response_handler() {
    Serial.println("RMB_CNG RES_H");
    constexpr Command_enum command=REMINDERB_CH;
    send_response_ACK(command);

    if(get_response(command)==SUCCESS)return true;
    return false;
}

extern Net_resource_post_remB_stat net_resource_post_remB_stat;
bool CommunicationHandler::reminderB_send_log_request_handler() {
    Serial.println("RMB_LOG - REQ_H");
    constexpr Command_enum command = REMINDERB_SND_LOG;
    if(send_response_SYN_ACK(command)!=ACK) {
        close_session(command);
        return false;
    }

    const JsonDocument reminder_log = receive_jsonDocument(command);
    if(reminder_log.size()==0) {
        close_session(command);
        return false;
    }
    net_resource_post_remB_stat.start_request(reminder_log);
    send_status_SUCCESS(command);
    return true;
}

bool CommunicationHandler::reminderB_send_log_response_handler() {
    Serial.println("RMB_LOG RES_H");
    constexpr Command_enum command=REMINDERB_SND_LOG;
    send_response_ACK(command);

    if(get_response(command)==SUCCESS)return true;
    return false;
}

void CommunicationHandler::send_command_reminderB_send_log() {
    send_request_SYN(REMINDERB_SND_LOG);
}









