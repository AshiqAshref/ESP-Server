#include "Network_communications.h"

#include <Command_deactivate_ap.h>
#include <Command_get_network_inf.h>
#include <Command_server_ip.h>
#include <HTTPClient.h>
#include <Output.h>
#include <Memmory.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <Net_resource_get_reminder_B.h>
#include <Net_resource_POST_remB_stat.h>
#include <Net_resource_remb_auto_update.h>
#include <SD.h>
#include <NTPClient.h>

extern NTPClient timeClient;
extern Error_Codes error_codes;
extern AsyncWebServer server;
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


extern Net_resource_post_remB_stat  net_resource_post_remB_stat;
extern Net_resource_get_reminder_B net_resource_get_reminder_B;
extern Net_resource_get_rev_no_B net_resource_get_rev_no_B;
extern Net_resource_remb_auto_update net_resource_remb_auto_update;
constexpr byte net_resource_size = 4;
Net_resource *net_resource[net_resource_size] = {
	&net_resource_post_remB_stat,
	&net_resource_get_reminder_B,
	&net_resource_get_rev_no_B,
	&net_resource_remb_auto_update
};




void Network_communications::handle_network_comms() {
	for(int i=0;i<net_resource_size;i++) {
		if(
			net_resource[i]->status()==NET_FAILED||
			net_resource[i]->status()==NET_COMPLETED_REFRESH
		){
			if(millis()-net_resource[i]->last_millis>net_resource[i]->retry_interval_) {
				net_resource[i]->start_request();
			}
		}
	}
	if(!error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		timeClient.update();
	}
}


bool Network_communications::resource_post_remb_stat(JsonDocument remb_log_json) {
	const String request_location= "modeB/esp/remb_log";
	const String server_address=  command_server_ip.server_address()+request_location;

	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}

	WiFiClient client;
	HTTPClient http;

	http.begin(client,   server_address); //HTTP
	const int r_code = http.GET();
	if (r_code == HTTP_CODE_OK) {
		error_codes.remove_error(SERVER_ERROR);
		return true;
	}
	error_codes.add_error(SERVER_ERROR);
	return false;
}


bool Network_communications::server_conn_test_local() {
	const String request_location= "modeB/test";
	const String server_address=  command_server_ip.server_address()+request_location;

	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}

	WiFiClient client;
	HTTPClient http;

	http.begin(client,   server_address); //HTTP
	const int r_code = http.GET();
	if (r_code == HTTP_CODE_OK) {
		error_codes.remove_error(SERVER_ERROR);
		return true;
	}
	error_codes.add_error(SERVER_ERROR);
	return false;
}


bool Network_communications::resource_get_revision_number(uint32_t &revision_no_) {
	const String request_location= "modeB/revision_no";
	const String server_address= command_server_ip.server_address()+request_location;

	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}

	bool (*start_req)(HTTPClient&, uint32_t&) = [](HTTPClient &http, uint32_t &revision_no) {
		http.addHeader("Content-Type", "application/json");
		const int r_code = http.GET();
		if (r_code == HTTP_CODE_OK) {
			JsonDocument response_revision_no;
			const DeserializationError error =  deserializeJson(response_revision_no, http.getString());
			if(error) {
				Serial.print("Json error: ");
				Serial.println(error.c_str());
				return false;
			}
			revision_no = response_revision_no["rno"].as<uint32_t>();
			Serial.print("got revision_no :");
			return true;
		}
		return false;
	};

	WiFiClient client_;
	HTTPClient http_;

	http_.begin(client_,   server_address); //HTTP
	const bool status = start_req(http_, revision_no_);
	client_.println("HTTP/1.1 200 OK");
	http_.end();
	client_.stop();
	status?
		error_codes.remove_error(SERVER_ERROR):
		error_codes.add_error(SERVER_ERROR);
	return status;
}


bool Network_communications::resource_get_reminder_B(uint32_t& revision_no_) {
	const String request_location = "modeB/esp/reminders/all";
	const String server_address=  command_server_ip.server_address()+request_location;

	if(error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		error_codes.add_error(SERVER_ERROR);
		return false;
	}

	bool (*start_req)(HTTPClient&, uint32_t&) = [](HTTPClient &http, uint32_t &revision_no) {
		http.addHeader("Content-Type", "application/json");
		const int r_code = http.GET();
		Serial.print("HTTP_CODE: ");
		Serial.println(r_code);
		if (r_code == HTTP_CODE_OK) {
			JsonDocument response_reminder_b;
			const DeserializationError error = deserializeJson(response_reminder_b, http.getString());
			if (error) {
				Serial.print("Json error: ");
				Serial.println(error.c_str());
				return false;
			}
			revision_no = response_reminder_b["revNo"]["rno"].as<uint32_t>();
			Memmory::write_reminders_to_SD(response_reminder_b["remB"]);
			Memmory::save_reminder_b_revision_no(revision_no);
			serializeJson(Memmory::get_all_reminders_from_sd(),Serial);
			return true;
		}
		return false;
	};

	WiFiClient client_;
	HTTPClient http_;
	http_.begin(client_, server_address); //HTTP
	const bool status = start_req(http_, revision_no_);
	client_.println("HTTP/1.1 200 OK");
	http_.end();
	client_.stop();
	status?
		error_codes.remove_error(SERVER_ERROR):
		error_codes.add_error(SERVER_ERROR);
	return status;
}

bool testing_new_ip=false;
constexpr byte new_ip_test_limit=20;
byte current_new_ip_test_count = 0;
bool Network_communications::server_conn_test() {
	const auto status = server_conn_test_local();
	Output::draw_server_icon(status);
	if(status && testing_new_ip) {
		Memmory::save_server_ip(command_server_ip.server_ip());
		testing_new_ip=false;
		current_new_ip_test_count=0;
		return status;
	}
	if(testing_new_ip && current_new_ip_test_count<new_ip_test_limit) {
		current_new_ip_test_count++;
		return status;
	}if(testing_new_ip) {
		Output::println("Switching to old ip");
		testing_new_ip=false;
		current_new_ip_test_count=0;
		command_server_ip.set_server_ip(Memmory::get_server_ip());
		return status;
	}
	return status;
}




bool Network_communications::initializeWiFi() {//......................INIT_WIFI
	WiFiClass::mode(WIFI_STA);
	Output::draw_AP_active_icon(false);
	initialize_self_server();


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
	return true;
}


bool Network_communications::initializeMDNS() {
	if(!error_codes.check_if_error_exist(WIFI_CONN_ERROR)) {
		if(MDNS.begin("esp32")) {
			Serial.println("MDNS :\"esp32\"");
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

void Network_communications::connection_end_protocol(const IPAddress &ip) {
	Serial.print("SERVER REMOTE IP: ");
	Serial.println(ip);
	if(error_codes.check_if_error_exist(SERVER_ERROR)) {
		Serial.print("Trying new IP: ");
		Serial.println(ip);
		command_server_ip.set_server_ip(ip);
		testing_new_ip = true;
		if(server_conn_test()) {
			Memmory::save_server_ip(ip);
		}
	}
}

void Network_communications::initialize_self_server() {
	server.end();
	server.on("/revision_change",HTTP_POST, [](AsyncWebServerRequest *request){}, nullptr, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
		Serial.println("REVISION CHANGE");
		String body="";
		for(size_t i=0; i<len; i++)body+=static_cast<char>(data[i]);
		Serial.println();
		Serial.print("Data : ");
		Serial.println(body);
		AsyncWebServerResponse *response = request->beginResponse(200);
		response->addHeader("Connection","close");
		request->send(response);
		connection_end_protocol(request->client()->getRemoteAddress());
		request->client()->close();
		net_resource_remb_auto_update.start_request();
	});

	server.on("/",HTTP_GET, [](AsyncWebServerRequest *request) {
		Serial.println("ESTABLISH CONN SERV");
		AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", getWifiIP().toString());
		response->addHeader("Connection","close");
		request->send(response);
		connection_end_protocol(request->client()->getRemoteAddress());

	});
	server.begin();
}


void Network_communications::initialize_AP_server() {
	server.end();
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
		Output::draw_AP_active_icon(false);
		command_deactivate_ap.send_request();
		initializeWiFi();
	});
	server.begin();
}

IPAddress Network_communications::setAccessPoint(){//.....................SET_ACCESSPOINT
	Output::println("Setting AP");
	WiFi.softAP("WIFI-MANAGER", nullptr);
	const IPAddress IP = WiFi.softAPIP();
	Output::print("AP_IP: ");
	Output::println(IP.toString(),false);
	Output::draw_AP_active_icon();
	Output::draw_Wifi_icon(4);
	initialize_AP_server();
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
		initialize_self_server();
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


void Network_communications::req_toString(AsyncWebServerRequest *request) {
	Serial.println("PRINT_START");
	Serial.println(request->methodToString());

	Serial.print("----ARGS---- : ");
	Serial.println(request->args());
	for(size_t i=0;i<request->args();i++) Serial.println(request->arg(i));

	Serial.println();
	Serial.print("----HEADERS---- : ");
	Serial.println(request->headers());
	for(size_t i=0;i<request->headers();i++) Serial.println(request->header(i));

	Serial.println();
	Serial.print("----HEADERS_PARAM---- : ");
	Serial.println(request->headers());
	for(size_t i=0;i<request->headers();i++) {
		Serial.print(request->getHeader(i)->name());
		Serial.print(" :: ");
		Serial.println(request->getHeader(i)->value());
	}

	Serial.println();
	Serial.print("----PARAMS---- : ");
	Serial.println(request->params());
	for(size_t i = 0;i<request->params();i++) {
		Serial.print(request->getParam(i)->name());
		Serial.print(" :: ");
		Serial.print(request->getParam(i)->value());
		Serial.print(" :: ");
		Serial.println(request->getParam(i)->size());
	}
	Serial.println("PRINT_END");
	Serial.println();
}

