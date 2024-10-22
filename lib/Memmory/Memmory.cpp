#include "Memmory.h"

#include <AV_Functions.h>
#include <Error_Codes.h>
#include <SD.h>
#include <Output.h>
#include <StreamUtils.h>

extern Error_Codes error_codes;
extern Output output;


byte extractHour(const String &formated_time) {
	return formated_time.substring(0, 2).toInt();
}
byte extractMinute(const String &formated_time) {
	return formated_time.substring(3, 5).toInt();
}
String Memmory::timetoString(const DateTime& t){
	return AV_Functions::beautifyTime(t.hour())+":"+ AV_Functions::beautifyTime(t.minute());
}
JsonDocument Memmory::get_latest_Reminder(const String &time_string, JsonDocument &doc){
	const auto t = DateTime(0,0,0,extractHour(time_string), extractMinute(time_string),0);
	return get_latest_Reminder(t, doc);
}
JsonDocument Memmory::get_latest_Reminder(const DateTime &t, JsonDocument &doc){
	const String current_time_string = AV_Functions::beautifyTime(t.hour())+':'+AV_Functions::beautifyTime(t.minute());
	const size_t total_reminders = doc.size();
	for(size_t i=0;i<total_reminders;i++){
		auto reminder_time_string = doc[i]["time"].as<String>();
		if(current_time_string<reminder_time_string){
			Serial.println(reminder_time_string);
			return get_reminder(doc, i);
		}
	}
	Serial.println(doc[0]["time"].as<String>());
	return get_reminder(doc, 0);
}
JsonDocument Memmory::get_latest_Reminder(const unsigned long unixTime, JsonDocument &doc){
	const auto t = DateTime(unixTime);
	return get_latest_Reminder(t, doc);
}
JsonDocument Memmory::get_reminder(JsonDocument &json_array, const byte position) {
	JsonDocument doc;
	// doc["s"].size()
	String val = json_array[position];
	deserializeJson(doc, val);
	json_array.clear();
	return doc;
}



JsonDocument Memmory::get_all_reminders_from_sd()  {
	File file = SD.open(reminderBfile,FILE_READ);
	JsonDocument doc;
	if(file){
		ReadBufferingStream bufferingStream(file, 64);
		const DeserializationError error = deserializeMsgPack(doc,bufferingStream);
		// const DeserializationError error = deserializeMsgPack(doc, file);
		String val = "medicines";
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
void Memmory::write_reminders_to_SD(JsonDocument doc){
	File file = SD.open(reminderBfile,FILE_WRITE);
	if(file){
		serializeMsgPack(doc, file);
		doc.clear();
		file.flush();
	}else{
		Serial.println("FE:");
	}
	file.close();
}
void Memmory::writeFile(const String &path, const char *message, const char *mode){//..............WRITE_FILE_SD
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
String Memmory::readFile(const String& path){//....................READ_FILE_SD
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
	File file =  SD.open(path,"r");
	String line =readLine(file, line_no);
	file.close();
	return line;
}
void Memmory::sd_print_all_files(const String& path) {
	File file = SD.open(path, "r");
	Output::println("--printBgn--");
	while(file.available())
		Output::print(static_cast<char>(file.read()),false);
	file.close();
	Output::println("--printEnd--");
}
void Memmory::save_wifi_cred(const String& ssid_, const String& pass_) {
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
	if(error_codes.check_if_error_exist(SD_CARD_ERROR)>-1) {
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
