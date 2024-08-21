#include "main.h"
#include <Arduino.h>

#include <RTClib.h>
#include <SoftwareSerial.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>

// #include <WiFiClient.h>
#include <NTPClient.h>
#include <SDFS.h>
#include <ReminderA.h>
#include <ReminderB.h>
#include <ArduinoJson.h>
#include <communication_protocols.h>
#include <Error_Codes.h>
//SD MODULE:  D7 MOSI, D6 MISO, D5 CLK, D0 CS
//RTC MODULE D1 SCL, D2 SDA
//SOFT SERRIAL D4 TX, D8 RX

#define SERIAL_TX 2 //d4
#define SERIAL_RX 15 //d8

auto reminderA = ReminderA();
auto reminderB = ReminderB();

const String wifiConfigFile = "/WifiConfig.dat";
const String WIFI_SSID_JSON_KEY = "wifi_SSID";
const String WIFI_PASS_JSON_KEY = "wifi_PASS";
String WIFI_SSID="Guest";
String WIFI_PASS="vFM95xht";

const String dataPath = "/dat.txt";
const String modePath = "/mode.txt";
const String modeBdat = "/modeBdat.txt";

const String PARAM_INPUT_1 = "ssid";
const String PARAM_INPUT_2 = "pass";

constexpr int SDFSchipSelect = 16; //D0
boolean isModeA=false;

SoftwareSerial hardwarePort(SERIAL_RX,SERIAL_TX);
AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,"time.windows.com",36000);
auto comms = Communication_protocols();
auto error_codes = Error_Codes();

void setup() {
	Serial.begin(9600);
	Serial.println("Started");
	Serial.println("Line1");
	if(!initializeSDFS()) {error_codes.add_error(SD_CARD_ERROR);}
	if(!load_wifi_cred()) {error_codes.add_error(BAD_WIFI_CRED);}
	if(!initializeWiFi()) {error_codes.add_error(WIFI_CONN_ERROR);}
	timeClient.begin();
	error_codes.print_all_errors();
	if(!initializeHardwareSerial()) {error_codes.add_error(SOFT_SERIAL_ERROR);}
}


bool load_wifi_cred() {
	if(!SDFS.exists(wifiConfigFile)) return false;
	File file = SDFS.open(wifiConfigFile,"r");
	JsonDocument doc;
	bool success=true;

	deserializeJson(doc,file);
	if (!doc.containsKey(WIFI_SSID_JSON_KEY) || !doc.containsKey(WIFI_PASS_JSON_KEY)) {
		success = false;
	}else if(doc[WIFI_SSID_JSON_KEY].as<String>().length()<1 || doc[WIFI_PASS_JSON_KEY].as<String>().length()<8){
		success = false;
	}else{
		WIFI_SSID = doc[WIFI_SSID_JSON_KEY].as<String>();
		WIFI_PASS = doc[WIFI_PASS_JSON_KEY].as<String>();
	}
	file.close();
	doc.clear();
	return success;
}
void save_wifi_cred(const String& ssid_, const String& pass_) {
	JsonDocument doc;
	doc[WIFI_SSID_JSON_KEY] = ssid_;
	doc[WIFI_PASS_JSON_KEY] = pass_;
	File file = SDFS.open(wifiConfigFile,"w+");
	serializeJson(doc,file);
	file.flush();
	file.close();
	doc.clear();
}

void loop() {
	MDNS.update();
	comms.handle_communications();

	// JsonDocument doc;
	// deserializeJson(doc, getReminders());
	// serializeJson(doc, hardwarePort);
	// hardwarePort.print(static_cast<String>(" i: ") + static_cast<String>(i++));
	// delay(2000);
}

boolean initializeWiFi() {//......................INIT_WIFI
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
	Serial.println("Connecting to WiFi...");

	const unsigned long curMil = millis();
	while(WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		if(millis()-curMil>=10000){
			return false;
		}
		delay(250);
	}

	Serial.println();
	Serial.print("Ip Addr: ");
	Serial.println(WiFi.localIP());
	Serial.print("HostName: ");
	Serial.println(WiFi.hostname());
	digitalWrite(LED_BUILTIN, 0);
	initializeMDNS();

	return true;
}


bool initializeMDNS() {
	if(MDNS.begin("esp32")) {
		Serial.println("MDNS SUccess ");
		return true;
	}
	Serial.println("MDNS fail");
	return false;
}


String beautifyTime(const uint8_t h_m_s) {
	if(h_m_s<10)
		return '0'+static_cast<String>(h_m_s);
	return static_cast<String>(h_m_s);
}


String get_formated_Time(const DateTime &curr_time, const byte mode) {
	if(mode == 12)
		return
			beautifyTime(curr_time.twelveHour())+":"
			+beautifyTime(curr_time.minute())+":"
			+beautifyTime(curr_time.second())+" "
			+(curr_time.isPM()? "p": " ");
	return
		beautifyTime(curr_time.hour())+":"
		+beautifyTime(curr_time.minute()) +":"
		+beautifyTime(curr_time.second());
}


boolean initializeHardwareSerial(){
	// hardwarePort.begin(9600, SWSERIAL_8N1, SERIAL_RX, SERIAL_TX, false);
	hardwarePort.begin(9600);
	if (!hardwarePort) { // If the object did not initialize, then its configuration is invalid
		Serial.println("Invalid SoftwareSerial pin configuration, check config");
		return false;
	}
	return true;
}

void writeFile(const String &path, const char *message, const char *mode){//..............WRITE_FILE_SD
	Serial.print("Writing file: "+ path);
	File file = SDFS.open(path, mode);
	if(!file){
		Serial.println("- failed to open file for writing");
		// return;
	}
	if(file.print(message)){
		Serial.println("- file written");
	} else {
		Serial.println("- write failed");
	}
	file.close();
}

String readFile(const String& path){//....................READ_FILE_SD
	Serial.println("Reading file: "+ String(path));

	File file = SDFS.open(path,"r");
	if(!file || file.isDirectory()){
		Serial.println("- failed to open file for reading");
		return "";
	}

	String fileContent;
	while(file.available()){
		fileContent = file.readStringUntil('\n');
		break;
	}
	file.close();
	return fileContent;
}

String readLine(File file) {
	if(file.available())
		return file.readStringUntil(10);
	return "";

}

String readLine(File file, const byte line_no) {
	file.seek(0,SeekSet);
	for(byte i=0;i<line_no;i++)
		if(file.available())
			file.readStringUntil(10);
	if(file.available())
		return file.readStringUntil(10);
	return "";
}

String readLine(const String& path, const byte line_no) {
	File file =  SDFS.open(path,"r");
	String line =readLine(file, line_no);
	file.close();
	return line;
}

void printAll(const String& path) {
	File file = SDFS.open(path, "r");
	Serial.println("--printBgn--");
	while(file.available())
		Serial.print(static_cast<char>(file.read()));
	file.close();
	Serial.println();
	Serial.println("--printEnd--");
}

void setAccessPoint(){//.....................SET_ACCESSPOINT
	Serial.println("Setting AP");
	WiFi.softAP("WIFI-MANAGER", nullptr);

	const IPAddress IP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(IP);

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
			request->send(SDFS, "data/wifimanager.html",String(), false);
	});
	server.serveStatic("/", SDFS, "data/");

	server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
		const size_t params = request->params();
		for(size_t i=0;i<params;i++){
			if(const AsyncWebParameter* p = request->getParam(i); p->isPost()){
				if (p->name() == PARAM_INPUT_1) {
					WIFI_SSID = p->value().c_str();
					Serial.print("SSID set to: ");
					Serial.println(WIFI_SSID);
				}
				if (p->name() == PARAM_INPUT_2) {
					WIFI_PASS = p->value().c_str();
					Serial.print("Password set to: ");
					Serial.println(WIFI_PASS);
				}
			}
		}
		request->send(200, "text/plain", "Reconnecting WiFi");
		if(initializeWiFi())server.end();
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
	File file=SDFS.open("dat.txt","r");
	auto result=ReminderA();
	if(file){
		Serial.println("File Opened");
		DateTime prev;
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
		Serial.println("File Open FAILED");
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
	File file=SDFS.open("modeBdat.txt","r");
	auto result=ReminderB();
	if(file){
		Serial.println("File Opened");
		DateTime prev;
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
		Serial.println("File Open FAILED");
	}
	return result;
}


String handle_index_modeB(const String &remoteIp){//.........................HANDLE_INDEX
	WiFiClient client;
	HTTPClient http;

	const String serverip=remoteIp+":8080/ESP_Manager/modeB";

	Serial.print("HTTP begin...\n");
	http.begin(client,  "http://"+ serverip+ "?espGetAll"); //HTTP
	http.addHeader("Content-Type", "text/plain");

	Serial.print("HTTP POST...\n");

	if (const int httpCode = http.POST("totalItems"); httpCode > 0) {
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
					Serial.println(temp);

					if(startFlag){
						const auto w="w";
						writeFile(modeBdat, temp.c_str(), w);
						startFlag=false;
					}else{
						const auto a="a";
						writeFile(modeBdat, temp.c_str(), a);
					}
					temp="";
					delay(100);
				}
			}
			Serial.println(payload);
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

	Serial.print("HTTP begin...\n");
	http.begin(client, "http://"+ serverip+ "?ESPGetAll"); //HTTP
	http.addHeader("Content-Type", "text/plain");

	Serial.print("HTTP POST...\n");

	if (const int httpCode = http.POST("totalItems"); httpCode > 0) {
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
					Serial.println(temp);

					if(startFlag){
						auto const w="w";
						writeFile(dataPath, temp.c_str(), w);
						startFlag=false;
					}else{
						auto const a="a";
						writeFile(dataPath, temp.c_str(), a);
					}
					temp="";
					delay(100);
				}
			}
			Serial.println(payload);
		}
	}else{
		Serial.printf("HTTP POST... failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
	}

	http.end();
	return "OK";
}


bool initializeSDFS() {
	SDFSConfig cfg; //SD card Module
	cfg.setCSPin(SDFSchipSelect);
	SDFS.setConfig(cfg);
	return SDFS.begin();
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



// boolean initializeRTC(){
//   DS1307_RTC.begin(); //RTC Module
//   // // DS1307_RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
//   DS1307_RTC.adjust(DateTime(0,0,0,0,16,45));
//   return true;
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
//     Serial.println("We Have Connection!");
//   }else{
//     Serial.println("Couldnt Connect to the Network");
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
//     Serial.print("RemoteIp..: ");
//     Serial.println(remoteIp);
//     Serial.print("RemotePort: ");
//     Serial.println(client.remotePort());
//
//     while (client.connected()){
//       if (client.available()){
//         Serial.print("Got SomeThing: ");
//         String line = client.readString();
//         Serial.println(line);
//         Serial.println("-----");
//         Serial.println();
//         client.println("HTTP/1.1 200 OK");
//         client.println();
//         client.println("DONE");
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
//     // Serial.print("Time Now: ");
//     // Serial.print(String(rtc.hour())+" : "+String(rtc.minute())+" : "+String(rtc.second()));
//
//     if(isModeA){
//       // Serial.print(" Upcomming--> ");
//       // Serial.print(reminder.Upc.hour());
//       // Serial.print(":");
//       // Serial.print(reminder.Upc.minute());
//       // Serial.print(" isModeA: ");
//       // Serial.println(isModeA);
//
//       if(DateTime(0,0,0,rtc.hour(),rtc.minute())==DateTime(0,0,0,reminderA.get_date_time().hour(),reminderA.get_date_time().minute())){
//         Serial.println("<<ALARMMMMMM>>");
//         if(alarmFlag){
//           alarmFlag=false;
//           String t="{ALM0:"+reminderA.toString()+"}";
//
//           for(int i=0;i<10;i++){
//             hardwarePort.write('.');
//             delay(50);
//           }
//           hardwarePort.write(t.c_str());
//           reminderA=checkUpcomming(DateTime(0,0,0,rtc.hour(),rtc.minute(),(rtc.second()+1)));
//           t="{UPC0:"+reminderA.toString()+"}";
//
//           for(int i=0;i<2;i++){
//             hardwarePort.write('.');
//             delay(50);
//           }
//           hardwarePort.write(t.c_str());
//         }else{
//           alarmFlag=true;
//         }
//       }
//     }else if(!isModeA){
//       // Serial.print(" Upcomming--> ");
//       // Serial.print(reminderB.Upc.hour());
//       // Serial.print(":");
//       // Serial.print(reminderB.Upc.minute());
//       // Serial.print(" isModeB: ");
//       // Serial.println(isModeA);
//
//       if(DateTime(0,0,0,rtc.hour(),rtc.minute())==DateTime(0,0,0,reminderB.get_date_time().hour(),reminderB.get_date_time().minute())){
//         Serial.println("<<ALARMMMMMM>> ModeB");
//         if(alarmFlag){
//           alarmFlag=false;
//           String t="{ALM1:"+reminderB.toString()+"}";
//
//           for(int i=0;i<10;i++){
//             hardwarePort.write('.');
//             delay(50);
//           }
//           hardwarePort.write(t.c_str());
//           reminderB=checkUpcommingB(DateTime(0,0,0,rtc.hour(),rtc.minute(),(rtc.second()+1)));
//           t="{UPC1:"+reminderB.toString()+"}";
//
//           for(int i=0;i<2;i++){
//             hardwarePort.write('.');
//             delay(50);
//           }
//           hardwarePort.write(t.c_str());
//         }else{
//           alarmFlag=true;
//         }
//       }
//     }
//   }
// }

