#include "main.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Adafruit_SSD1306.h>

#include <Command_activate_AP.h>
#include <Command_deactivate_ap.h>
#include <Command_get_network_inf.h>
#include <Command_get_reminderB.h>
#include <Command_get_time.h>
#include <Command_daylight_sav.h>
#include <Command_server_ip.h>
#include <Command_reminderB_change.h>

#include <ReminderA.h>
#include <ReminderB.h>
#include <Error_Codes.h>
#include <CommunicationHandler.h>
#include <Memmory.h>
#include <Network_communications.h>
#include <Output.h>
#include <Communication_protocols.h>


auto reminderA = ReminderA();
auto reminderB = ReminderB();

String WIFI_SSID="";
String WIFI_PASS="";

boolean isModeA=false;

constexpr byte SCREEN_WIDTH =128; // OLED display width, in pixels
constexpr byte SCREEN_HEIGHT =64; // OLED display height, in pixels

auto command_get_time = Command_get_time(
		GET_TIME,
		[]{},
		[]{return true;},
		CommunicationHandler::NTP_request_handler,
		5000);
auto command_get_reminder_b = Command_get_reminderB (
		GET_REMINDER_B,
		[]{},
		[]{return true;},
		CommunicationHandler::reminder_b_request_handler,
		5000);
auto command_activate_AP = Command_activate_AP (
		ACTIVATE_AP,
		[]{},
		[]{return true;},
		CommunicationHandler::activate_AP_request_handler,
		5000);
auto command_deactivate_ap = Command_deactivate_ap(
		DEACTIVATE_AP,
		CommunicationHandler::send_command_deactivate_ap,
		CommunicationHandler::deactivate_AP_response_handler,
		CommunicationHandler::deactivate_AP_request_handler,
		5000);
auto command_get_network_inf = Command_get_network_inf(
		CommunicationHandler::send_command_get_network_inf,
		CommunicationHandler::get_network_inf_response_handler,
		CommunicationHandler::get_network_inf_request_handler,
		2000);
auto command_daylight_sav = Command_daylight_sav(
		CommunicationHandler::send_command_daylight_sav,
		CommunicationHandler::daylight_sav_response_handler,
		CommunicationHandler::daylight_sav_request_handler,
		10000);
auto command_server_ip = Command_server_ip(
		CommunicationHandler::send_command_server_ip,
		CommunicationHandler::server_ip_response_handler,
		CommunicationHandler::server_ip_request_handler,
		3000);
auto command_reminderB_change = Command_reminderB_change(
		CommunicationHandler::send_command_reminderB_change,
		CommunicationHandler::reminderB_change_response_handler,
		CommunicationHandler::reminderB_change_request_handler,
		4000);

auto server= AsyncWebServer(80);
auto ntpUDP=WiFiUDP();
auto timeClient = NTPClient(ntpUDP,"time.windows.com",36000);
auto oled=Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT,&Wire,-1);
auto error_codes = Error_Codes();
auto comms = Communication_protocols();

void setup() {
	Serial.begin(115200);
	if(!Output::initializeOLED()) {error_codes.add_error(OLED_ERROR);}
	if(!Memmory::initializeSDFS()) {error_codes.add_error(SD_CARD_ERROR);}
	if(!Memmory::load_wifi_cred(WIFI_SSID,WIFI_PASS)) {error_codes.add_error(BAD_WIFI_CRED);}
	if(!Network_communications::initializeWiFi()) {error_codes.add_error(WIFI_CONN_ERROR);}
	if(!Network_communications::initializeMDNS()) {error_codes.add_error(MDNS_ERROR);}
	if(!Network_communications::initializeNTP())  {error_codes.add_error(NTP_ERROR);}
	command_server_ip.set_server_ip(Memmory::get_server_ip());
	if(!Network_communications::server_conn_test()) {error_codes.add_error(SERVER_ERROR);}
	if(!CommunicationHandler::initializeHardwareSerial()) {error_codes.add_error(SOFT_SERIAL_ERROR);}
	Output::print_all_errors();
}

void loop() {
	CommunicationHandler::handle_communications();
	Network_communications::handle_network_comms();
	resolve_errors();
	updateUiComponents();
}

constexpr unsigned int RSSI_update_interval=3000;
unsigned int last_RSSI_update_millis=0;

void updateUiComponents(){
	if(millis()-last_RSSI_update_millis>RSSI_update_interval){
		last_RSSI_update_millis=millis();
		if(WiFiClass::status() == WL_CONNECTED) {
			Output::update_Wifi_RSSI_ICO(WiFi.RSSI());
		}else {
			Output::draw_Wifi_icon(4);
			error_codes.add_error(WIFI_CONN_ERROR);
			command_get_network_inf.send_request();
		}
	}
}


void resolve_errors() {
	if(error_codes.total_errors()>0) {
		for(byte i=0;i<error_codes.total_errors();i++) {
			if(resolve_error(error_codes.get_error(i))) {
				i=0;
				Output::print_all_errors();
			}
		}
	}
}

constexpr unsigned long server_conn_test_delay=3000;
unsigned long last_server_conn_test_millis=0;
bool resolve_error(const INTERNAL_ERROR_CODE error_code) {
	if(error_code==WIFI_CONN_ERROR) {
		Output::draw_Wifi_icon(4);
		if(Network_communications::resolve_WIFI_CONN_ERROR()) {
			last_RSSI_update_millis = 0;
			if(!Network_communications::initializeMDNS()) {error_codes.add_error(MDNS_ERROR);}
			if(!Network_communications::initializeNTP())  {error_codes.add_error(NTP_ERROR);}
			return true;
		}
		return false;
	}if(error_code==NTP_ERROR) {
		return Network_communications::initializeNTP();
	}if(error_code==MDNS_ERROR) {
		return Network_communications::initializeMDNS();
	}if(error_code==SD_CARD_ERROR) {
		return Memmory::resolve_SD_CARD_ERROR();
	}if(error_code==BAD_WIFI_CRED) {
		return Network_communications::resolve_BAD_WIFI_CRED();
	}if(error_code==SOFT_SERIAL_ERROR) {
		return CommunicationHandler::initializeHardwareSerial();
	}if(error_code==SERVER_ERROR) {
		if(millis()-last_server_conn_test_millis<server_conn_test_delay) {
			return false;
		}last_server_conn_test_millis=millis();
		return Network_communications::server_conn_test();
	}
	return false;
}