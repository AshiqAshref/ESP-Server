#include "main.h"
#include <Arduino.h>
#include <RTClib.h>
#include <SD.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFiUdp.h>
// #include <WiFiClient.h>
#include <NTPClient.h>
#include <ReminderA.h>
#include <ReminderB.h>
#include <ArduinoJson.h>
#include <communication_protocols.h>
#include <Error_Codes.h>
#include <ESPmDNS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Output.h>
#include <Memmory.h>

auto reminderA = ReminderA();
auto reminderB = ReminderB();

const String wifiConfigFile = "/WifiConfig.dat";

// String WIFI_SSID="Guest";
// String WIFI_PASS="vFM95xht";
String WIFI_SSID="";
String WIFI_PASS="";

const String dataPath = "/dat.txt";
const String modePath = "/mode.txt";
const String modeBdat = "/modeBdat.txt";

const String PARAM_INPUT_1 = "ssid";
const String PARAM_INPUT_2 = "pass";
bool tryNewPass=false;

boolean isModeA=false;

constexpr byte SCREEN_WIDTH =128; // OLED display width, in pixels
constexpr byte SCREEN_HEIGHT =64; // OLED display height, in pixels

auto server = AsyncWebServer(80);
auto ntpUDP=WiFiUDP();
auto timeClient = NTPClient(ntpUDP,"time.windows.com",36000);
auto oled=Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT,&Wire,-1);
auto error_codes = Error_Codes();
auto comms = Communication_protocols();
auto output=Output();
auto memmory = Memmory(wifiConfigFile);

void setup() {
	Serial.begin(115200);
	Serial.println("Started");
	Serial.println("Line1");
	if(!initializeOLED()) {error_codes.add_error(OLED_ERROR);}
	if(!Memmory::initializeSDFS()) {error_codes.add_error(SD_CARD_ERROR);}
	if(!memmory.load_wifi_cred(WIFI_SSID,WIFI_PASS)) {error_codes.add_error(BAD_WIFI_CRED);}
	if(!initializeWiFi()) {error_codes.add_error(WIFI_CONN_ERROR);}
	if(!initializeMDNS()) {error_codes.add_error(MDNS_ERROR);}
	if(!initializeNTP())  {error_codes.add_error(NTP_ERROR);}
	if(!initializeHardwareSerial()) {error_codes.add_error(SOFT_SERIAL_ERROR);}
	Output::print_all_errors();

}

void loop() {
	comms.handle_communications();
	resolve_errors();
	updateUiComponents();

	// JsonDocument doc;
	// deserializeJson(doc, getReminders());
	// serializeJson(doc, Serial1);
	// Serial1.print(static_cast<String>(" i: ") + static_cast<String>(i++));
	// delay(2000);
}

unsigned int prev_RSSI_update=0;
constexpr unsigned int RSSI_update_interval=3000;
void updateUiComponents(){
	if(millis()-prev_RSSI_update>RSSI_update_interval){
		prev_RSSI_update=millis();
		if(WiFiClass::status() == WL_CONNECTED) {
			output.update_Wifi_RSSI_ICO(WiFi.RSSI());
		}else {
			output.draw_Wifi_icon(4);
			error_codes.add_error(WIFI_CONN_ERROR);
		}
	}
}

unsigned long previous_reconnect_millis=0;
constexpr unsigned int reconnect_interval=60000;
bool resolve_WIFI_CONN_ERROR() {
	if(WiFiClass::status() == WL_CONNECTED) {
		error_codes.remove_error(WIFI_CONN_ERROR);
		Output::print("IP: ");
		Output::println(WiFi.localIP().toString(), false);
		Output::print("ID: ");
		Output::println(WiFiClass::getHostname(), false);
		if(tryNewPass) {
			memmory.save_wifi_cred(WIFI_SSID,WIFI_PASS);
			output.draw_AP_active_icon(false);
			tryNewPass=false;
		}
		return true;
	}if(error_codes.check_if_error_exist(BAD_WIFI_CRED)>-1) {
		return memmory.load_wifi_cred(WIFI_SSID,WIFI_PASS);
	}if(error_codes.check_if_error_exist(BAD_WIFI_CRED)>-1) {
		return false;
	}if(error_codes.check_if_error_exist(SD_CARD_ERROR)>-1) {
		return false;
	}if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)>-1) {
		if ((WiFiClass::status() != WL_CONNECTED) && (millis() -  previous_reconnect_millis>=reconnect_interval)) {
			WiFi.disconnect();
			WiFi.reconnect();
			previous_reconnect_millis = millis();
		}
	}
	return false;
}
bool resolve_SD_CARD_ERROR() {
	output.draw_SD_eror_icon();
	SD.end();
	if(Memmory::initializeSDFS()) {
		error_codes.remove_error(SD_CARD_ERROR);
		output.draw_SD_eror_icon(false);
		return true;
	}
	return false;
}
bool resolve_MDNS_ERROR() {
	MDNS.end();
	return initializeMDNS();
}
bool resolve_BAD_WIFI_CRED() {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR))
		return false;
	return memmory.load_wifi_cred(WIFI_SSID,WIFI_PASS);
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
		output.draw_Wifi_icon(4);
		if(resolve_WIFI_CONN_ERROR()) {
			prev_RSSI_update = 0;
			initializeNTP();
			initializeMDNS();
			return true;
		}
		return false;
	}if(error_code==NTP_ERROR) {
		return initializeNTP();
	}if(error_code==MDNS_ERROR) {
		return initializeMDNS();
	}if(error_code==SD_CARD_ERROR) {
		return resolve_SD_CARD_ERROR();
	}if(error_code==BAD_WIFI_CRED) {
		return resolve_BAD_WIFI_CRED();
	}if(error_code==SOFT_SERIAL_ERROR) {
		return initializeHardwareSerial();
	}
	return false;
}
bool initializeOLED() {
	if(oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
		oled.cp437();
		oled.clearDisplay();
		oled.setTextSize(1);
		oled.setTextColor(WHITE);
		Output::println("Start");
		error_codes.remove_error(OLED_ERROR);
		return true;
	}
	error_codes.add_error(OLED_ERROR);
	return false;
}

bool initializeWiFi() {//......................INIT_WIFI
	WiFiClass::mode(WIFI_STA);
	WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
	WiFi.disconnect();
	WiFi.reconnect();
	
	Output::println("Connecting to WiFi...");
	const unsigned long curMil = millis();
	byte a = 0;
	while(WiFiClass::status() != WL_CONNECTED) {
		Output::print('.',false);
		if(millis()-curMil>=10000){
			Output::println();
			error_codes.add_error(WIFI_CONN_ERROR);
			return false;
		}
		output.animateConnection(a);
		delay(500);
	}
	
	Output::print("IP: ");
	Output::println(WiFi.localIP().toString(),false);
	Output::print("ID: ");
	Output::println(WiFiClass::getHostname(),false);
	error_codes.remove_error(WIFI_CONN_ERROR);
	if(tryNewPass) {
		memmory.save_wifi_cred(WIFI_SSID,WIFI_PASS);
		server.end();
		output.draw_AP_active_icon(false);
		tryNewPass=false;
	}
	return true;
}
bool initializeMDNS() {
	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)<0) {
		if(MDNS.begin("esp32")) {
			error_codes.remove_error(MDNS_ERROR);
			return true;
		}
	}
	error_codes.add_error(MDNS_ERROR);
	return false;
}
bool initializeNTP() {
	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)<0){
		timeClient.begin();
		timeClient.forceUpdate();
		if(timeClient.isTimeSet()) {
			error_codes.remove_error(NTP_ERROR);
			return true;
		}
	}
	error_codes.add_error(NTP_ERROR);
	return false;
}
bool initializeHardwareSerial(){
	Serial1.begin(115200,SERIAL_8N1,16,17);
	if (!Serial1) {
		error_codes.add_error(SOFT_SERIAL_ERROR);
		return false;
	}
	error_codes.remove_error(SOFT_SERIAL_ERROR);
	return true;
}



void setAccessPoint(){//.....................SET_ACCESSPOINT
	Output::println("Setting AP");
	WiFi.softAP("WIFI-MANAGER", nullptr);
	const IPAddress IP = WiFi.softAPIP();
	Output::print("AP_IP: ");
	Output::println(IP.toString(),false);
	output.draw_AP_active_icon();

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
			request->send(SD, "/data/wifimanager.html","text/html");
	});
	server.serveStatic("/", SD, "/data/");

	server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
		const size_t params = request->params();
		for(size_t i=0;i<params;i++){
			const AsyncWebParameter* p = request->getParam(i);
			if(p->isPost()){
				if (p->name() == PARAM_INPUT_1) {
					WIFI_SSID = p->value();
					Output::print("NEW_SSID: ");
					Output::println(WIFI_SSID, false);
					tryNewPass=true;
				}if (p->name() == PARAM_INPUT_2) {
					WIFI_PASS = p->value();
					Output::print("NEW_PASS: ");
					Output::println(WIFI_PASS, false);
					tryNewPass=true;
				}
			}
		}
		request->send(200, "text/plain", "Reconnecting WiFi");
		server.end();
		initializeWiFi();
	});
	server.begin();
}

ReminderA jsonToClass(String& dat){
	unsigned int id=dat.substring((dat.indexOf(':')+1),dat.indexOf(',')).toInt();

	dat=dat.substring(dat.indexOf(',')+1);
	const String date=dat.substring(dat.indexOf(':')+1,dat.indexOf(','));

	const byte H=date.substring(1,3).toInt();

	const byte M=date.substring(4,6).toInt();

	dat=dat.substring(dat.indexOf(',')+1);
	byte boxNo=dat.substring((dat.indexOf(':')+2),(dat.indexOf(',')-1)).toInt();

	return {DateTime(0,0,0,H,M,0), boxNo, id, false};
}

ReminderA checkUpcomming(const DateTime& currentTime){ //............................CHECK_UPCOMMING
	File file=SD.open("dat.txt","r");
	auto result=ReminderA();
	if(file){
		Output::println("File Opened");
		// DateTime prev;
		boolean startFlag=true;
		auto prevReminder=ReminderA();

		while(file.available()){
			String dat=file.readStringUntil('\n');
			result=jsonToClass(dat);
			auto input= DateTime(0,0,0,result.get_date_time().hour(), result.get_date_time().minute(),0);

			if(currentTime>input){
				if(startFlag){
					while(file.available()){
						String dat1=file.readStringUntil('\n');
						if(!file.available()){
							result=jsonToClass(dat1);
							file.close();
							return result;
						}
					}
				}
				result=prevReminder;
				file.close();
				return result;
			}
			startFlag=false;
			prevReminder=result;
		}
	}else{
		Output::println("File Open FAILED");
	}
	return result;
}

ReminderB jsonToClassB(const String& dat) {
	const byte H = dat.substring((dat.indexOf('=') + 1), dat.indexOf(':')).toInt();
	const byte M = dat.substring((dat.indexOf(':') + 1), dat.indexOf('\"')).toInt();
	String boxesString = dat.substring((dat.indexOf(',') + 1));
	boxesString = boxesString.substring(dat.indexOf('=') + 1);
	boxesString = boxesString.substring(0, boxesString.length()-1);

	return {DateTime(0, 0, 0, H, M, 0), boxesString, false};
}

ReminderB checkUpcommingB(const DateTime& currentTime){ //............................CHECK_UPCOMMING
	File file=SD.open("modeBdat.txt","r");
	auto result=ReminderB();
	if(file){
		Output::println("File Opened");
		// DateTime prev;
		boolean startFlag=true;
		auto prevReminder=ReminderB();

		while(file.available()){
			String dat=file.readStringUntil('\n');
			result=jsonToClassB(dat);
			auto input= DateTime(0,0,0,result.get_date_time().hour(), result.get_date_time().minute(),0);

			if(currentTime>input){
				if(startFlag){
					while(file.available()){
						String dat1=file.readStringUntil('\n');
						if(!file.available()){
							result=jsonToClassB(dat1);
							file.close();
							return result;
						}
					}
				}
				result=prevReminder;
				file.close();
				return result;
			}
			startFlag=false;
			prevReminder=result;
		}
	}else{
		Output::println("File Open FAILED");
	}
	return result;
}

String handle_index_modeB(const String &remoteIp){//.........................HANDLE_INDEX
	WiFiClient client;
	HTTPClient http;

	const String serverip=remoteIp+":8080/ESP_Manager/modeB";

	Output::print("HTTP begin...\n");
	http.begin(client,  "http://"+ serverip+ "?espGetAll"); //HTTP
	http.addHeader("Content-Type", "text/plain");

	Output::print("HTTP POST...\n");
	const int httpCode = http.POST("totalItems");
	if ( httpCode > 0) {
		Serial.printf("HTTP POST... code: %d\n", httpCode);

		if (httpCode == HTTP_CODE_OK) {
			const String& payload = http.getString();
			String temp;
			boolean aFlag=false;
			boolean startFlag=true;

			for(unsigned int i=0; i<payload.length(); i++){
				if(payload.charAt(i)=='{'){
					aFlag=true;
					temp=payload.charAt(i);
				}else if(aFlag && payload.charAt(i)!='}' ){
					temp+=payload.charAt(i);
				}else if(aFlag && payload.charAt(i)=='}'){
					aFlag=false;
					temp+=payload.charAt(i);
					temp+="\n";
					Output::println(temp);

					if(startFlag){
						const auto w="w";
						Memmory::writeFile(modeBdat, temp.c_str(), w);
						startFlag=false;
					}else{
						const auto a="a";
						Memmory::writeFile(modeBdat, temp.c_str(), a);
					}
					temp="";
					delay(100);
				}
			}
			Output::println(payload);
		}
	}else{
		Serial.printf("HTTP POST... failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
	}

	http.end();
	return "OK";
}

String handle_index(const String &remoteIp){//.........................HANDLE_INDEX
	WiFiClient client;
	HTTPClient http;
	const String serverip=remoteIp+":8080/ESP_Manager/data";

	Output::print("HTTP begin...\n");
	http.begin(client, "http://"+ serverip+ "?ESPGetAll"); //HTTP
	http.addHeader("Content-Type", "text/plain");

	Output::print("HTTP POST...\n");
	const int httpCode = http.POST("totalItems");
	if ( httpCode > 0) {
		Serial.printf("HTTP POST... code: %d\n", httpCode);
		if (httpCode == HTTP_CODE_OK) {
			const String& payload = http.getString();
			String temp;
			boolean aFlag=false;
			boolean startFlag=true;

			for(unsigned int i=0; i<payload.length(); i++){
				if(payload.charAt(i)=='{'){
					aFlag=true;
					temp=payload.charAt(i);
				}else if(aFlag && payload.charAt(i)!='}' ){
					temp+=payload.charAt(i);
				}else if(aFlag && payload.charAt(i)=='}'){
					aFlag=false;
					temp+=payload.charAt(i);
					temp+="\n";
					Output::println(temp);

					if(startFlag){
						auto const w="w";
						Memmory::writeFile(dataPath, temp.c_str(), w);
						startFlag=false;
					}else{
						auto const a="a";
						Memmory::writeFile(dataPath, temp.c_str(), a);
					}
					temp="";
					delay(100);
				}
			}
			Output::println(payload);
		}
	}else{
		Serial.printf("HTTP POST... failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
	}

	http.end();
	return "OK";
}


String getReminders() {
	String json =R"(
		[
				{
						"timeId": 24,
						"time": "00:10",
						"medicines": [
								{
										"medBox": 9,
										"dosage": 1,
										"success": false
								}
						]
				},
				{
						"timeId": 6,
						"time": "00:20",
						"medicines": [
								{
										"medBox": 14,
										"dosage": 2,
										"success": false
								},
								{
										"medBox": 9,
										"dosage": 3,
										"success": false
								}
						]
				},
				{
						"timeId": 4,
						"time": "10:05",
						"medicines": [
								{
										"medBox": 7,
										"dosage": 3,
										"success": false
								}
						]
				},
				{
						"timeId": 3,
						"time": "20:45",
						"medicines": [
								{
										"medBox": 7,
										"dosage": 2,
										"success": false
								},
								{
										"medBox": 14,
										"dosage": 4,
										"success": false
								}
						]
				}
		])";
	return json;
}

// void addRemindes() {
//     String json =R"(
//     [
//         {
//             "timeId": 24,
//             "time": "00:10",
//             "medicines": [
//                 {
//                     "medBox": 9,
//                     "dosage": 1,
//                     "success": false
//                 }
//             ]
//         },
//         {
//             "timeId": 6,
//             "time": "00:20",
//             "medicines": [
//                 {
//                     "medBox": 14,
//                     "dosage": 2,
//                     "success": false
//                 },
//                 {
//                     "medBox": 9,
//                     "dosage": 3,
//                     "success": false
//                 }
//             ]
//         },
//         {
//             "timeId": 4,
//             "time": "10:05",
//             "medicines": [
//                 {
//                     "medBox": 7,
//                     "dosage": 3,
//                     "success": false
//                 }
//             ]
//         },
//         {
//             "timeId": 3,
//             "time": "20:45",
//             "medicines": [
//                 {
//                     "medBox": 7,
//                     "dosage": 2,
//                     "success": false
//                 },
//                 {
//                     "medBox": 14,
//                     "dosage": 4,
//                     "success": false
//                 }
//             ]
//         }
//     ])";
// }

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

