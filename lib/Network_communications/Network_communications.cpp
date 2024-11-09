#include "Network_communications.h"

#include <Command_deactivate_ap.h>
#include <Command_get_network_inf.h>
#include <Command_server_ip.h>
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
extern Command_server_ip command_server_ip;



const String modeBdat = "/modeBdat.txt";
const String dataPath = "/dat.txt";

const String PARAM_INPUT_1 = "ssid";
const String PARAM_INPUT_2 = "pass";

bool tryNewPass=false;
unsigned long last_reminder_b_revision_no=0;

// constexpr unsigned int server_conn_test_interval=120000; TEST CODE
constexpr unsigned int handle_data_interval=120000;
unsigned long last_data_handle_millis=0;
void Network_communications::handle_network_comms() {
	WiFiClient client = server.available();
	if(client) {handle_client(client);}

	if(millis()-last_data_handle_millis>handle_data_interval) {
		handle_data();
		last_data_handle_millis=millis();
	}
}

bool Network_communications::handle_data() {
	const bool status = get_revision_number();
	Output::draw_server_icon(status);
	if(status) {
		if(Memmory::get_reminder_b_revision_no()!=last_reminder_b_revision_no) {
			if(get_reminder_B()) {
				Memmory::set_reminder_b_revision_no(last_reminder_b_revision_no);
				serializeJson(Memmory::get_all_reminders_from_sd(),Serial);
				Serial.println();
			}
		}
	}
	return status;
}

constexpr unsigned long server_conn_test_delay=3000;
unsigned long last_server_conn_test_millis=0;
bool Network_communications::server_conn_test() {
	const auto status = server_conn_test_local();
	Output::draw_server_icon(status);
	return status;
}

bool Network_communications::server_conn_test_local() {
	const String request_location= ":8080/modeB/test";
	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}if(millis()-last_server_conn_test_millis<server_conn_test_delay) {
		return false;
	}last_server_conn_test_millis=millis();

	WiFiClient client;
	HTTPClient http;
	const String server_address= "http://"+ command_server_ip.server_ip().toString()+request_location;

	http.begin(client,   server_address); //HTTP
	http.addHeader("Content-Type", "application/json");
	const int r_code = http.GET();
	if (r_code == HTTP_CODE_OK) {
		error_codes.remove_error(SERVER_ERROR);
		return true;
	}
	error_codes.add_error(SERVER_ERROR);
	return false;
}


void Network_communications::handle_client(WiFiClient client) {
	const String remoteIp=client.remoteIP().toString();
	const IPAddress ip = IPAddress().fromString(remoteIp);

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
			if(error_codes.check_if_error_exist(SERVER_ERROR)) {
				command_server_ip.set_server_ip(ip);
				if(server_conn_test()) {
					Memmory::set_server_ip(ip);
				}
			}
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
	// command_get_network_inf.send_request();
	ap_server.end();
	server.begin();
	return true;
}

bool Network_communications::initializeMDNS() {
	if(!error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		if(MDNS.begin("esp32")) {
			Serial.print("MDNS :\"esp32\"");
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




bool Network_communications::get_revision_number() {
	const String request_location= ":8080/modeB/revision_no";

	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}

	WiFiClient client;
	HTTPClient http;

	const String server_address= "http://"+ command_server_ip.server_ip().toString()+request_location;
	// const String server_address= "http://host.wokwi.internal"+request_location;

	http.begin(client,   server_address); //HTTP
	http.addHeader("Content-Type", "application/json");
	const int r_code = http.GET();
	if (r_code == HTTP_CODE_OK) {
		error_codes.remove_error(SERVER_ERROR);

		JsonDocument response_revision_no;
		const DeserializationError error =  deserializeJson(response_revision_no, http.getString());
		if(error) {
			Serial.print("Json error: ");
			Serial.println(error.c_str());
			error_codes.add_error(SERVER_ERROR);
			return false;
		}
		last_reminder_b_revision_no = response_revision_no["revisionNo"].as<uint32_t>();
		Serial.print("got revision_no :");
		Serial.println(last_reminder_b_revision_no);
		return true;
	}
	error_codes.add_error(SERVER_ERROR);
	return false;
}

bool Network_communications::get_reminder_B() {
	const String request_location = ":8080/modeB/esp/reminders/all";
	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}

	WiFiClient client;
	HTTPClient http;
	const String server_address= "http://"+ command_server_ip.server_ip().toString()+request_location;
	// const String server_address= "http://host.wokwi.internal"+request_location;
	http.begin(client,   server_address); //HTTP
	http.addHeader("Content-Type", "application/json");
	const int r_code = http.GET();
	Serial.print("HTTP_CODE: ");
	Serial.println(r_code);
	if (r_code == HTTP_CODE_OK) {
		error_codes.remove_error(SERVER_ERROR);
		JsonDocument response_reminder_b;
		const DeserializationError error =  deserializeJson(response_reminder_b, http.getString());

		if(error) {
			Serial.print("Json error: ");
			Serial.println(error.c_str());
			error_codes.add_error(SERVER_ERROR);
			return false;
		}
		Memmory::write_reminders_to_SD(response_reminder_b);
		return true;
	}
	error_codes.add_error(SERVER_ERROR);
	return false;
}

//
// String Network_communications::handle_index(const String &remoteIp){//.........................HANDLE_INDEX
// 	WiFiClient client;
// 	HTTPClient http;
// 	const String serverip=remoteIp+":8080/ESP_Manager/data";
//
// 	Output::print("HTTP begin...\n");
// 	http.begin(client, "http://"+ serverip+ "?ESPGetAll"); //HTTP
// 	http.addHeader("Content-Type", "text/plain");
//
// 	Output::print("HTTP POST...\n");
// 	const int httpCode = http.POST("totalItems");
// 	if ( httpCode > 0) {
// 		Serial.printf("HTTP POST... code: %d\n", httpCode);
// 		if (httpCode == HTTP_CODE_OK) {
// 			const String& payload = http.getString();
// 			String temp;
// 			boolean aFlag=false;
// 			boolean startFlag=true;
//
// 			for(unsigned int i=0; i<payload.length(); i++){
// 				if(payload.charAt(i)=='{'){
// 					aFlag=true;
// 					temp=payload.charAt(i);
// 				}else if(aFlag && payload.charAt(i)!='}' ){
// 					temp+=payload.charAt(i);
// 				}else if(aFlag && payload.charAt(i)=='}'){
// 					aFlag=false;
// 					temp+=payload.charAt(i);
// 					temp+="\n";
// 					Output::println(temp);
//
// 					if(startFlag){
// 						auto const w="w";
// 						Memmory::writeFile(dataPath, temp.c_str(), w);
// 						startFlag=false;
// 					}else{
// 						auto const a="a";
// 						Memmory::writeFile(dataPath, temp.c_str(), a);
// 					}
// 					temp="";
// 					delay(100);
// 				}
// 			}
// 			Output::println(payload);
// 		}
// 	}else{
// 		Serial.printf("HTTP POST... failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
// 	}
//
// 	http.end();
// 	return "OK";
// }
