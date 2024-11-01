#include "Network_communications.h"

#include <Command_deactivate_ap.h>
#include <Command_get_network_inf.h>
#include <HTTPClient.h>
#include <Output.h>
#include <Memmory.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <NTPClient.h>

extern NTPClient timeClient;
extern Error_Codes error_codes;
extern AsyncWebServer ap_server;
extern WiFiServer server;
extern String WIFI_SSID;
extern String WIFI_PASS;
extern Command_deactivate_ap command_deactivate_ap;
extern Command_get_network_inf command_get_network_inf;


const String modeBdat = "/modeBdat.txt";
const String dataPath = "/dat.txt";

const String PARAM_INPUT_1 = "ssid";
const String PARAM_INPUT_2 = "pass";

bool tryNewPass=false;

void Network_communications::handle_network_comms() {
	WiFiClient client = server.available();
	if (client) {
		handle_client(client);
	}
}

void Network_communications::handle_client(WiFiClient client) {
	const String remoteIp=client.remoteIP().toString();
	Output::print("RemoteIp..: ");
	Output::println(remoteIp);
	Output::print("RemotePort: ");
	Output::println(client.remotePort());

	while (client.connected()){
		if (client.available()){
			Output::print("Got SomeThing: ");
			String line = client.readString();
			Output::println(line);
			client.println("HTTP/1.1 200 OK");
			client.println();
			client.println("DONE");
			client.stop();
			handle_index_modeB(remoteIp);
		}
	}
}

bool Network_communications::initializeWiFi() {//......................INIT_WIFI
	WiFiClass::mode(WIFI_STA);
	Output::draw_AP_active_icon(false);
	ap_server.end();
	server.begin();

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
			Output::draw_Wifi_icon(4);
			error_codes.add_error(WIFI_CONN_ERROR);
			server.end();
			return false;
		}
		Output::animateConnection(a);
		delay(500);
	}

	Output::print("IP: ");
	Output::println(WiFi.localIP().toString(),false);
	Output::print("ID: ");
	Output::println(WiFiClass::getHostname(),false);
	error_codes.remove_error(WIFI_CONN_ERROR);
	if(tryNewPass) {
		Memmory::save_wifi_cred(WIFI_SSID,WIFI_PASS);
		tryNewPass=false;
	}
	command_get_network_inf.send_request();
	ap_server.end();
	server.begin();
	server.begin();
	return true;
}

bool Network_communications::initializeMDNS() {
	if(!error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		if(MDNS.begin("esp32")) {
			error_codes.remove_error(MDNS_ERROR);
			return true;
		}
	}
	error_codes.add_error(MDNS_ERROR);
	return false;
}

bool Network_communications::initializeNTP() {
	if(!error_codes.check_if_error_exist(WIFI_CONN_ERROR)){
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

IPAddress Network_communications::setAccessPoint(){//.....................SET_ACCESSPOINT
	Output::println("Setting AP");
	WiFi.softAP("WIFI-MANAGER", nullptr);
	// command_get_network_inf.send_request();
	const IPAddress IP = WiFi.softAPIP();
	Output::print("AP_IP: ");
	Output::println(IP.toString(),false);
	Output::draw_AP_active_icon();
	Output::draw_Wifi_icon(4);


	ap_server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
			request->send(SD, "/data/wifimanager.html","text/html");
	});
	ap_server.serveStatic("/", SD, "/data/");

	ap_server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
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
		ap_server.end();
		Output::draw_AP_active_icon(false);
		command_deactivate_ap.send_request();
		initializeWiFi();
	});
	server.end();
	ap_server.begin();
	return IP;
}

IPAddress Network_communications::getAPIP() {
	return WiFi.softAPIP();
}
IPAddress Network_communications::getWifiIP() {
	return WiFi.localIP();
}
bool Network_communications::wifiConnected() {
	return WiFiClass::status() == WL_CONNECTED;
}

unsigned long previous_reconnect_millis=0;
constexpr unsigned int reconnect_interval=60000;
bool Network_communications::resolve_WIFI_CONN_ERROR() {
	if(WiFiClass::status() == WL_CONNECTED) {
		error_codes.remove_error(WIFI_CONN_ERROR);
		Output::print("IP: ");
		Output::println(WiFi.localIP().toString(), false);
		Output::print("ID: ");
		Output::println(WiFiClass::getHostname(), false);
		server.end();
		server.begin();
		if(tryNewPass) {
			Memmory::save_wifi_cred(WIFI_SSID,WIFI_PASS);
			Output::draw_AP_active_icon(false);
			tryNewPass=false;
		}
		return true;
	}if(error_codes.check_if_error_exist(BAD_WIFI_CRED)) {
		return Memmory::load_wifi_cred(WIFI_SSID,WIFI_PASS);
	}if(error_codes.check_if_error_exist(BAD_WIFI_CRED)) {
		return false;
	}if(error_codes.check_if_error_exist(SD_CARD_ERROR)) {
		return false;
	}if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		if ((WiFiClass::status() != WL_CONNECTED) && (millis() -  previous_reconnect_millis>=reconnect_interval)) {
			WiFi.disconnect();
			WiFi.reconnect();
			previous_reconnect_millis = millis();
		}
	}
	return false;
}


bool Network_communications::resolve_MDNS_ERROR() {
	MDNS.end();
	return initializeMDNS();
}

bool Network_communications::resolve_BAD_WIFI_CRED() {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR))return false;
	return Memmory::load_wifi_cred(WIFI_SSID,WIFI_PASS);
}

String Network_communications::handle_index_modeB(const String &remoteIp){//.........................HANDLE_INDEX
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

String Network_communications::handle_index(const String &remoteIp){//.........................HANDLE_INDEX
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
