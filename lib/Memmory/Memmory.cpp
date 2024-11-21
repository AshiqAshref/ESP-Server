#include "Memmory.h"

#include <AV_Functions.h>
#include <Error_Codes.h>
#include <SD.h>
#include <Output.h>
#include <StreamUtils.h>

extern Error_Codes error_codes;
extern Output output;

JsonDocument Memmory::get_latest_Reminder(const String &time_string, JsonDocument &doc){
	const auto t = DateTime(2020,12,12,AV_Functions::extractHour(time_string), AV_Functions::extractMinute(time_string),0);
	return get_latest_Reminder(t, doc);
}
JsonDocument Memmory::get_latest_Reminder(const DateTime &t, JsonDocument &doc){
	const auto current_time = DateTime(2020,12,12, t.hour(),t.minute(),0);
	const size_t total_reminders = doc.size();
	for(size_t i=0;i<total_reminders;i++){
		auto reminder_time_string = doc[i]["t"].as<String>();
		const auto reminder_time = DateTime(2020,12,12,AV_Functions::extractHour(reminder_time_string),AV_Functions::extractMinute(reminder_time_string),0);
		if(current_time.unixtime()<reminder_time.unixtime()){
			Serial.println(reminder_time_string);
			return get_reminder(doc, i);
		}
	}
	Serial.println(doc[0]["t"].as<String>());
	return get_reminder(doc, 0);
}
JsonDocument Memmory::get_latest_Reminder(const unsigned long unixTime, JsonDocument &doc){
	const auto t = DateTime(unixTime);
	return get_latest_Reminder(t, doc);
}
JsonDocument Memmory::get_latest_Reminder(const unsigned long unixTime){
	const auto t = DateTime(unixTime);
	JsonDocument doc = get_all_reminders_from_sd();
	return get_latest_Reminder(t, doc);
}
JsonDocument Memmory::get_reminder(JsonDocument &json_array, const byte position) {
	JsonDocument doc;
	String val = json_array[position];
	deserializeJson(doc, val);
	json_array.clear();
	return doc;
}



JsonDocument Memmory::get_all_reminders_from_sd()  {
	JsonDocument doc;
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return doc;
	File file = SD.open(reminderBfile,FILE_READ);
	if(file){
		ReadBufferingStream bufferingStream(file, 64);
		const DeserializationError error = deserializeMsgPack(doc,bufferingStream);
		if(error){
			Serial.print("DE:");
			Serial.println(error.c_str());
		}
	}else{
		Serial.println("File Error2");
	}
	file.close();
	return doc;
}


bool Memmory::initializeSDFS() {
    if(SD.begin()) {
        error_codes.remove_error(SD_CARD_ERROR);
        return true;
    }
    error_codes.add_error(SD_CARD_ERROR);
    Output::draw_SD_eror_icon();
    return false;
}

bool Memmory::write_reminders_to_SD(const JsonDocument &reminders_json){
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return false;
	JsonDocument doc = AV_Functions::simplify_Json(reminders_json);
	if(doc.size()==0)
		return false;
	File file = SD.open(reminderBfile,FILE_WRITE);
	if(file){
		serializeMsgPack(doc, file);
		doc.clear();
		file.flush();
	}else{
		return false;
	}
	file.close();
	return true;
}

void Memmory::writeFile(const String &path, const char *message, const char *mode){//..............WRITE_FILE_SD
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return;
	Output::print("Writing file: "+ path);
	File file = SD.open(path, mode);
	if(!file){
		Output::println("- failed to open file for writing");
	}
	if(file.print(message)){
		Output::println("- file written");
	} else {
		Output::println("- write failed");
	}
	file.close();
}


bool Memmory::get_daylight_saving() {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return false;
	if(!SD.exists(daylight_saving_file)) {
		writeFile(daylight_saving_file, "false", FILE_WRITE);
		return false;
	}
	String dlt_stat=readLine(daylight_saving_file,0);
	dlt_stat.trim();
	Serial.print("GET_DLS read_line: ");
	Serial.println(dlt_stat);
	if(dlt_stat.equals("true"))return true;
	return false;
}
void Memmory::save_daylight_saving(const bool dlt_sv) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return;
	String dlt_stat=readLine(daylight_saving_file,0);
	dlt_stat.trim();
	if(dlt_stat.equals("true") && dlt_sv==false)
		writeFile(daylight_saving_file, "false", FILE_WRITE);
	else if(dlt_stat.equals("false") && dlt_sv==true)
		writeFile(daylight_saving_file, "true", FILE_WRITE);
	Serial.print("WRITTEN_DLS read_line: ");
	Serial.println(readLine(daylight_saving_file,0));
}


IPAddress Memmory::get_server_ip() {
	IPAddress ip(0,0,0,0);
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return ip;
	if(!SD.exists(server_ip_file)) {
		writeFile(server_ip_file, ip.toString().c_str(), FILE_WRITE);
		return ip;
	}
	String server_ip_string=readLine(server_ip_file,0);
	server_ip_string.trim();
	ip.fromString(server_ip_string);
	return ip;
}
void Memmory::save_server_ip(const IPAddress &server_ip) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return;
	String old_server_ip_string=readLine(server_ip_file,0);
	old_server_ip_string.trim();
	if(!old_server_ip_string.equals(server_ip.toString()))
		writeFile(server_ip_file, server_ip.toString().c_str(), FILE_WRITE);
}

uint32_t Memmory::get_reminder_b_revision_no() {
	unsigned long revision_no=0;
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return revision_no;
	if(!SD.exists(reminder_b_revision_file)) {
		char buf[16];
		ultoa(revision_no,buf,10);
		writeFile(reminder_b_revision_file, buf, FILE_WRITE);
		return revision_no;
	}
	String revision_no_string=readLine(reminder_b_revision_file,0);
	revision_no_string.trim();
	revision_no= strtol(revision_no_string.c_str(), nullptr, 10);
	return revision_no;
}
void Memmory::save_reminder_b_revision_no(const uint32_t revision_no) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return;
	String old_remb_rev_no_str=readLine(reminder_b_revision_file,0);
	old_remb_rev_no_str.trim();
	const u_int32_t old_remb_rev_no_long = strtol(old_remb_rev_no_str.c_str(), nullptr, 10);
	if(old_remb_rev_no_long!=revision_no) {
		char buf[16];
		ultoa(revision_no,buf,10);
		writeFile(reminder_b_revision_file, buf, FILE_WRITE);
	}
}


String Memmory::readFile(const String& path){//....................READ_FILE_SD
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return "";
	Output::println("Reading file: "+ String(path));
	File file = SD.open(path,"r");
	if(!file || file.isDirectory()){
		Output::println("- failed to open file for reading");
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
String Memmory::readLine(File file) {
	if(file.available())
		return file.readStringUntil(10);
	return "";

}
String Memmory::readLine(File file, const byte line_no) {
	file.seek(0,SeekSet);
	for(byte i=0;i<line_no;i++)
		if(file.available())
			file.readStringUntil(10);
	if(file.available())
		return file.readStringUntil(10);
	return "";
}
String Memmory::readLine(const String& path, const byte line_no) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return "";
	File file =  SD.open(path,"r");
	String line =readLine(file, line_no);
	file.close();
	return line;
}
void Memmory::sd_print_all_files(const String& path) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return;
	File file = SD.open(path, "r");
	Output::println("--printBgn--");
	while(file.available())
		Output::print(static_cast<char>(file.read()),false);
	file.close();
	Output::println("--printEnd--");
}
void Memmory::save_wifi_cred(const String& ssid_, const String& pass_) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) return;
	JsonDocument doc;
	doc[WIFI_SSID_JSON_KEY] = ssid_;
	doc[WIFI_PASS_JSON_KEY] = pass_;
	File file = SD.open(wifiConfigFile,"w+");
	serializeJson(doc,file);
	file.flush();
	file.close();
	doc.clear();
}
bool Memmory::load_wifi_cred(String &WIFI_SSID, String &WIFI_PASS) {
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)) {
		error_codes.add_error(BAD_WIFI_CRED);
		return false;
	}if(!SD.exists(wifiConfigFile)) {
		Output::println("ConfigFileNotFound");
		error_codes.add_error(BAD_WIFI_CRED);
		return false;
	}
	File file = SD.open(wifiConfigFile,"r");
	JsonDocument doc;
	bool success=true;

	deserializeJson(doc,file);
	if (!doc[WIFI_SSID_JSON_KEY].is<String>() || !doc[WIFI_PASS_JSON_KEY].is<String>()) {
		success = false;
	}else if(doc[WIFI_SSID_JSON_KEY].as<String>().length()<1 || doc[WIFI_PASS_JSON_KEY].as<String>().length()<8){
		success = false;
	}else{
		WIFI_SSID = doc[WIFI_SSID_JSON_KEY].as<String>();
		WIFI_PASS = doc[WIFI_PASS_JSON_KEY].as<String>();
	}
	file.close();
	doc.clear();
	if(success) error_codes.remove_error(BAD_WIFI_CRED);
	else error_codes.add_error(BAD_WIFI_CRED);
	return success;
}

bool Memmory::resolve_SD_CARD_ERROR() {
	Output::draw_SD_eror_icon();
	SD.end();
	if(initializeSDFS()) {
		error_codes.remove_error(SD_CARD_ERROR);
		Output::draw_SD_eror_icon(false);
		return true;
	}
	return false;
}

String Memmory::timetoString(const DateTime& t){
	return AV_Functions::beautifyTime(t.hour())+":"+ AV_Functions::beautifyTime(t.minute());
}