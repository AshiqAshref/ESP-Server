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

auto ap_server = AsyncWebServer(80);
auto server= WiFiServer(80);
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
	if(!CommunicationHandler::initializeHardwareSerial()) {error_codes.add_error(SOFT_SERIAL_ERROR);}
	Output::print_all_errors();
}

void loop() {
	CommunicationHandler::handle_communications();
	Network_communications::handle_network_comms();
	resolve_errors();
	updateUiComponents();
}

unsigned int prev_RSSI_update=0;
constexpr unsigned int RSSI_update_interval=3000;
void updateUiComponents(){
	if(millis()-prev_RSSI_update>RSSI_update_interval){
		prev_RSSI_update=millis();
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

bool resolve_error(const INTERNAL_ERROR_CODE error_code) {
	if(error_code==WIFI_CONN_ERROR) {
		Output::draw_Wifi_icon(4);
		if(Network_communications::resolve_WIFI_CONN_ERROR()) {
			prev_RSSI_update = 0;
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
	}
	return false;
}


// void setup() {  //.......................................................................SETUP
//   Serial.begin(9600);
//   initializeHardwareSerial();
//   initializeRTC();
//   initializeSDFS();
//   while (!Serial) {}
//
//   pinMode(LED_BUILTIN, OUTPUT);
//
//   ssid = readFile(ssidPath);
//   pass = readFile(passPath);
//
//   if(initializeWiFi()){
//     Output::println("We Have Connection!");
//   }else{
//     Output::println("Couldnt Connect to the Network");
//     setAccessPoint();
//   }
//
//   String b=readFile(modePath);
//   b.trim();
//
//   if(b.equalsIgnoreCase("MODE_B")){
//     DS1307_RTC.adjust(DateTime(0,0,0,0,16,55));
//     isModeA=false;
//     reminderB= checkUpcommingB(DateTime(0,0,0, DS1307_RTC.now().hour(), DS1307_RTC.now().minute(), DS1307_RTC.now().second()));
//   }else if(b.equalsIgnoreCase("MODE_A")){
//     DS1307_RTC.adjust(DateTime(0,0,0,17,04,55));
//     isModeA=true;
//     reminderA= checkUpcomming(DateTime(0,0,0, DS1307_RTC.now().hour(), DS1307_RTC.now().minute(), DS1307_RTC.now().second()));
//   }
//
//   server.begin();
//   ser.begin();
// }

// unsigned long currentMillis=0;
// boolean alarmFlag=true;
// void loop() {//.......................................LOOP
//   if (WiFiClient client = ser.accept()){
//     const String remoteIp=client.remoteIP().toString();
//     Output::print("RemoteIp..: ");
//     Output::println(remoteIp);
//     Output::print("RemotePort: ");
//     Output::println(client.remotePort());
//
//     while (client.connected()){
//       if (client.available()){
//         Output::print("Got SomeThing: ");
//         String line = client.readString();
//         Output::println(line);
//         Output::println("-----");
//         Output::println();
//         client.Output::println("HTTP/1.1 200 OK");
//         client.Output::println();
//         client.Output::println("DONE");
//         client.stop();
//         if(isModeA)
//           handle_index(remoteIp);
//         else
//           handle_index_modeB(remoteIp);
//       }
//     }
//   }
//
//   if(millis()-currentMillis>=1000){
//     currentMillis=millis();
//     const auto rtc=DateTime(0,0,0,DS1307_RTC.now().hour(), DS1307_RTC.now().minute(), DS1307_RTC.now().second());
//
//     // Output::print("Time Now: ");
//     // Output::print(String(rtc.hour())+" : "+String(rtc.minute())+" : "+String(rtc.second()));
//
//     if(isModeA){
//       // Output::print(" Upcomming--> ");
//       // Output::print(reminder.Upc.hour());
//       // Output::print(":");
//       // Output::print(reminder.Upc.minute());
//       // Output::print(" isModeA: ");
//       // Output::println(isModeA);
//
//       if(DateTime(0,0,0,rtc.hour(),rtc.minute())==DateTime(0,0,0,reminderA.get_date_time().hour(),reminderA.get_date_time().minute())){
//         Output::println("<<ALARMMMMMM>>");
//         if(alarmFlag){
//           alarmFlag=false;
//           String t="{ALM0:"+reminderA.toString()+"}";
//
//           for(int i=0;i<10;i++){
//             Serial1.write('.');
//             delay(50);
//           }
//           Serial1.write(t.c_str());
//           reminderA=checkUpcomming(DateTime(0,0,0,rtc.hour(),rtc.minute(),(rtc.second()+1)));
//           t="{UPC0:"+reminderA.toString()+"}";
//
//           for(int i=0;i<2;i++){
//             Serial1.write('.');
//             delay(50);
//           }
//           Serial1.write(t.c_str());
//         }else{
//           alarmFlag=true;
//         }
//       }
//     }else if(!isModeA){
//       // Output::print(" Upcomming--> ");
//       // Output::print(reminderB.Upc.hour());
//       // Output::print(":");
//       // Output::print(reminderB.Upc.minute());
//       // Output::print(" isModeB: ");
//       // Output::println(isModeA);
//
//       if(DateTime(0,0,0,rtc.hour(),rtc.minute())==DateTime(0,0,0,reminderB.get_date_time().hour(),reminderB.get_date_time().minute())){
//         Output::println("<<ALARMMMMMM>> ModeB");
//         if(alarmFlag){
//           alarmFlag=false;
//           String t="{ALM1:"+reminderB.toString()+"}";
//
//           for(int i=0;i<10;i++){
//             Serial1.write('.');
//             delay(50);
//           }
//           Serial1.write(t.c_str());
//           reminderB=checkUpcommingB(DateTime(0,0,0,rtc.hour(),rtc.minute(),(rtc.second()+1)));
//           t="{UPC1:"+reminderB.toString()+"}";
//
//           for(int i=0;i<2;i++){
//             Serial1.write('.');
//             delay(50);
//           }
//           Serial1.write(t.c_str());
//         }else{
//           alarmFlag=true;
//         }
//       }
//     }
//   }
// }

