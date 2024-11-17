#include "AV_Functions.h"
#include <RTClib.h>


void AV_Functions::waitForInput() {
    flush_();
    while(!Serial.available()) {}
    Serial.flush();
    while(Serial.available())Serial.read();
}

 void AV_Functions::flush_() {
    Serial.flush();
    while(Serial.available())Serial.read();
}

String AV_Functions::get_formated_Time(const DateTime &curr_time, const byte mode) {
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

String AV_Functions::beautifyTime(const uint8_t h_m_s) {
     if(h_m_s<10)
         return '0'+static_cast<String>(h_m_s);
     return static_cast<String>(h_m_s);
 }


void AV_Functions::printBin(const byte aByte) {
    Serial.print('(');
    for (int8_t aBit = 7; aBit >= 0; aBit--) {
        Serial.print(bitRead(aByte, aBit) ? '1' : '0');
        if(aBit==4) Serial.print(' ');
    }
    Serial.print(')');
}
void AV_Functions::printlnBin(const byte aByte) {
    printBin(aByte);
    Serial.println();
}


JsonDocument AV_Functions::simplify_Json(const JsonDocument &doc){
    JsonDocument main_doc;
    if(!isJsonValid(doc)) {
        JsonDocument empty_doc;
        return empty_doc;
    }

    auto main_doc_array= main_doc.to<JsonArray>();
    size_t doc_array_size = doc.size();
    for(size_t i=0; i<doc_array_size; i++){
        JsonDocument reminder_doc;
        reminder_doc["ti"] =doc[i]["timeId"];
        reminder_doc["t"] = doc[i]["time"].as<String>();

        auto med_array = reminder_doc["m"].to<JsonArray>();
        size_t med_array_size = doc[i]["medicines"].size();
        for (size_t j = 0; j < med_array_size; j++) {
            JsonDocument med_doc;
            med_doc["b"] = doc[i]["medicines"][j]["medBox"];
            med_doc["d"] = doc[i]["medicines"][j]["dosage"];
            med_doc["s"] = doc[i]["medicines"][j]["success"] ? 1 : 0;
            // ReSharper disable once CppExpressionWithoutSideEffects
            med_array.add(med_doc);
        }

        // ReSharper disable once CppExpressionWithoutSideEffects
        main_doc_array.add(reminder_doc);
    }
    return main_doc;
}


bool AV_Functions::isJsonValid(const JsonDocument &doc){
    const size_t doc_array_size = doc.size();
    if(doc_array_size==0)
        return false;
    for(size_t i=0; i<doc_array_size; i++){
        if(doc[i]["timeId"].is<int>()) {
        }else return false;
        if(doc[i]["time"].is<String>()) {
        }else return false;

        if(doc[i]["medicines"].size()>0) {
            const size_t med_array_size = doc[i]["medicines"].size();
            for (size_t j = 0; j < med_array_size; j++) {
                if (doc[i]["medicines"][j]["medBox"].is<int>()) {
                } else return false;
                if (doc[i]["medicines"][j]["dosage"].is<int>()) {
                } else return false;
                if (doc[i]["medicines"][j]["success"].is<bool>()) {
                } else return false;
            }
        } else return false;
    }
    return true;
}

JsonDocument AV_Functions::unsimplify_Json(const JsonDocument &doc){
    JsonDocument main_doc;
    JsonDocument empty_doc;

    auto main_doc_array= main_doc.to<JsonArray>();
    size_t doc_array_size = doc.size();
    if(doc_array_size==0)
        return empty_doc;
    for(size_t i=0; i<doc_array_size; i++){
        JsonDocument reminder_doc;
        if(doc[i]["ti"].is<int>()) {
            reminder_doc["timeId"] =doc[i]["ti"];
        }else return empty_doc;

        if(doc[i]["t"].is<String>()) {
            reminder_doc["time"] = doc[i]["t"].as<String>();
        }else return empty_doc;

        if(doc[i]["rv"].is<int>()) {
            reminder_doc["rv"] = doc[i]["rv"].as<String>();
        }else return empty_doc;

        auto med_array = reminder_doc["medicines"].to<JsonArray>();
        if(doc[i]["m"].size()>0) {
            size_t med_array_size = doc[i]["m"].size();
            for(size_t j=0; j<med_array_size; j++){
                JsonDocument med_doc;
                if(doc[i]["m"][j]["b"].is<int>()) {
                    med_doc["medBox"] = doc[i]["m"][j]["b"];
                }else return empty_doc;
                if(doc[i]["m"][j]["d"].is<int>()) {
                    med_doc["dosage"] = doc[i]["m"][j]["d"];
                }else return empty_doc;
                if(doc[i]["m"][j]["s"].is<int>()) {
                    med_doc["success"] = doc[i]["m"][j]["s"]?true:false;
                }else return empty_doc;
                // ReSharper disable once CppExpressionWithoutSideEffects
                med_array.add(med_doc);
            }
        }else return empty_doc;
        // ReSharper disable once CppExpressionWithoutSideEffects
        main_doc_array.add(reminder_doc);
    }
    return main_doc;
}


byte AV_Functions::extractHour(const String &formated_time) {
    return formated_time.substring(0, 2).toInt();
}
byte AV_Functions::extractMinute(const String &formated_time) {
    return formated_time.substring(3, 5).toInt();
}

