// ReSharper disable CppExpressionWithoutSideEffects
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

JsonDocument AV_Functions::simplify_Json_local(const JsonDocument &doc) {
    JsonDocument reminder_doc;
    if(doc["timeId"].is<int>())
        reminder_doc["ti"] =doc["timeId"];
    else reminder_doc["ti"] =0;

    if(doc["time"].is<String>())
        reminder_doc["t"] = doc["time"].as<String>();
    else reminder_doc["t"] = "";

    auto med_array = reminder_doc["m"].to<JsonArray>();
    size_t med_array_size = doc["medicines"].size();
    if(med_array_size>0) {
        for (size_t j = 0; j < med_array_size; j++) {
            JsonDocument med_doc;
            if (doc["medicines"][j]["medBox"].is<int>())
                med_doc["b"] = doc["medicines"][j]["medBox"];
            else med_doc["b"] = 0;

            if (doc["medicines"][j]["dosage"].is<int>())
                med_doc["d"] = doc["medicines"][j]["dosage"];
            else med_doc["d"] = 0;

            if (doc["medicines"][j]["success"].is<bool>())
                med_doc["s"] = doc["medicines"][j]["success"] ? 1 : 0;
            else med_doc["s"] = 0;

            // ReSharper disable once CppExpressionWithoutSideEffects
            med_array.add(med_doc);
        }
    }else {
        med_array.add(JsonDocument());
    }
    return reminder_doc;
}

JsonDocument AV_Functions::simplify_Json(JsonDocument doc){
    JsonDocument main_doc;
    size_t doc_array_size = doc.size();
    if(doc_array_size==0) return JsonDocument();
    if(doc.as<JsonArray>().isNull()) {
        main_doc = simplify_Json_local(doc);
    }else {
        auto main_doc_array= main_doc.to<JsonArray>();
        for(size_t i=0; i<doc_array_size; i++){
            JsonDocument reminder_doc=simplify_Json_local(doc[i]);
            main_doc_array.add(reminder_doc);
        }
    }
    return main_doc;
}




JsonDocument AV_Functions::unsimplify_Json_local(const JsonDocument &doc) {
    JsonDocument reminder_doc;
    if(doc["ti"].is<int>()) {
        reminder_doc["timeId"] =doc["ti"];
    }else reminder_doc["timeId"] = 0;

    if(doc["t"].is<String>()) {
        reminder_doc["time"] = doc["t"].as<String>();
    }else reminder_doc["time"] = "";

    if(doc["rv"].is<int>()) {
        reminder_doc["rv"] = doc["rv"].as<uint32_t>();
    }else reminder_doc["rv"] = 0;


    const auto med_array = reminder_doc["medicines"].to<JsonArray>();
    if(doc["m"].size()>0) {
        const size_t med_array_size = doc["m"].size();
        for(size_t j=0; j<med_array_size; j++){
            JsonDocument med_doc;
            if(doc["m"][j]["b"].is<int>()) {
                med_doc["medBox"] = doc["m"][j]["b"];
            }else med_doc["medBox"]= 0;

            if(doc["m"][j]["d"].is<int>()) {
                med_doc["dosage"] = doc["m"][j]["d"];
            }else med_doc["dosage"] = 0;

            if(doc["m"][j]["s"].is<int>()) {
                med_doc["success"] = doc["m"][j]["s"]?true:false;
            }else med_doc["success"]=false;
            med_array.add(med_doc);
        }
    }else {
        med_array.add(JsonDocument());
    }
    return reminder_doc;

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

JsonDocument AV_Functions::unsimplify_Json(JsonDocument doc){
    JsonDocument main_doc;
    const size_t doc_array_size = doc.size();
    if(doc_array_size==0) return JsonDocument();

    if(doc.as<JsonArray>().isNull()) {
        main_doc = unsimplify_Json_local(doc);
    }else {
        const auto main_doc_array= main_doc.to<JsonArray>();
        for(size_t i=0; i<doc_array_size; i++){
            JsonDocument reminder_doc = unsimplify_Json_local(doc[i]);
            main_doc_array.add(reminder_doc);
        }
    }
    return main_doc;
}


byte AV_Functions::extractHour(const String &formated_time) {
    return formated_time.substring(0, 2).toInt();
}
byte AV_Functions::extractMinute(const String &formated_time) {
    return formated_time.substring(3, 5).toInt();
}

