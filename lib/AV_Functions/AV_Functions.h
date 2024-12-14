// ReSharper disable CppExpressionWithoutSideEffects
#ifndef AV_FUNCTIONS_H
#define AV_FUNCTIONS_H
#include <Arduino.h>
#include <RTClib.h>
#include <ArduinoJson.h>

class AV_Functions {
    static JsonDocument unsimplify_Json_local(const JsonDocument &doc){
        JsonDocument reminder_doc;
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
                if(doc["m"][j]["b"].is<uint8_t>()) {
                    med_doc["medBox"] = doc["m"][j]["b"].as<uint8_t>();
                }else med_doc["medBox"]= 0;

                if(doc["m"][j]["d"].is<uint8_t>()) {
                    med_doc["dosage"] = doc["m"][j]["d"].as<uint8_t>();
                }else med_doc["dosage"] = 0;

                if(doc["m"][j]["n"].is<String>()) {
                    med_doc["mname"] = doc["m"][j]["n"].as<String>();
                }else med_doc["mname"]="";

                if(doc["m"][j]["a"].is<uint16_t>()) {
                    med_doc["mamount"] = doc["m"][j]["a"].as<uint16_t>();
                }else med_doc["mamount"] = 0;

                if(doc["m"][j]["ct"].is<uint32_t>()) {
                    med_doc["ctime"] = doc["m"][j]["tc"].as<uint32_t>();
                }else med_doc["ctime"]=0;

                med_array.add(med_doc);
            }
        }else {
            med_array.add(JsonDocument());
        }
        return reminder_doc;

    }
    static JsonDocument simplify_Json_local(const JsonDocument &doc){
        JsonDocument reminder_doc;

        if(doc["time"].is<String>())
            reminder_doc["t"] = doc["time"].as<String>();
        else reminder_doc["t"] = "";

        const auto med_array = reminder_doc["m"].to<JsonArray>();
        const size_t med_array_size = doc["medicines"].size();
        if(med_array_size>0) {
            for (size_t j = 0; j < med_array_size; j++) {
                JsonDocument med_doc;
                if (doc["medicines"][j]["medBox"].is<int>())
                    med_doc["b"] = doc["medicines"][j]["medBox"];
                else med_doc["b"] = 0;

                if (doc["medicines"][j]["dosage"].is<int>())
                    med_doc["d"] = doc["medicines"][j]["dosage"];
                else med_doc["d"] = 0;

                // ReSharper disable once CppExpressionWithoutSideEffects
                med_array.add(med_doc);
            }
        }else {
            med_array.add(JsonDocument());
        }
        return reminder_doc;
    }
    static JsonDocument simplify_box_Json_local(const JsonDocument &doc){
        JsonDocument reminder_doc;

        if(doc["mname"].is<String>())
            reminder_doc["n"] = doc["mname"].as<String>();
        else reminder_doc["n"] = "";

        if(doc["mamount"].is<uint16_t>())
            reminder_doc["a"] = doc["mamount"].as<uint16_t>();
        else reminder_doc["a"] = 0;

        if(doc["mbox"].is<uint8_t>())
            reminder_doc["b"] = doc["mbox"].as<uint8_t>();
        else reminder_doc["b"] = 0;

        return reminder_doc;
    }
    static JsonDocument unsimplify_box_Json_local(const JsonDocument &doc) {
        JsonDocument reminder_doc;
        if(doc["n"].is<String>()) {
            reminder_doc["mname"] = doc["n"].as<String>();
        }else reminder_doc["mname"] = "";

        if(doc["a"].is<uint16_t>()) {
            reminder_doc["mamount"] = doc["a"].as<uint16_t>();
        }else reminder_doc["mamount"] = "";

        if(doc["b"].is<uint8_t>()) {
            reminder_doc["mbox"] = doc["b"].as<uint8_t>();
        }else reminder_doc["mbox"] = 0;

        return reminder_doc;
    }



public:
    AV_Functions()=default;
    ~AV_Functions()=default;
    static void waitForInput(){
        flush_();
        while(!Serial.available()) {}
        Serial.flush();
        while(Serial.available())Serial.read();
    }

    static void flush_(){
        Serial.flush();
        while(Serial.available())Serial.read();
    }


    static String get_formated_Time(const DateTime &curr_time, const byte mode=12) {
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
    static String beautifyTime(const uint8_t h_m_s){
        if(h_m_s<10)
            return '0'+static_cast<String>(h_m_s);
        return static_cast<String>(h_m_s);
    }

    static void printBin(const byte aByte){
        Serial.print('(');
        for (int8_t aBit = 7; aBit >= 0; aBit--) {
            Serial.print(bitRead(aByte, aBit) ? '1' : '0');
            if(aBit==4) Serial.print(' ');
        }
        Serial.print(')');
    }
    static void printlnBin(const byte aByte){
        printBin(aByte);
        Serial.println();
    }


    static bool isJsonValid(const JsonDocument &doc){
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

    static JsonDocument simplify_Json(JsonDocument doc) {
        JsonDocument main_doc;
        const size_t doc_array_size = doc.size();
        if(doc_array_size==0) return JsonDocument();
        if(doc.as<JsonArray>().isNull()) {
            main_doc = simplify_Json_local(doc);
        }else {
            const auto main_doc_array= main_doc.to<JsonArray>();
            for(size_t i=0; i<doc_array_size; i++){
                JsonDocument reminder_doc=simplify_Json_local(doc[i]);
                main_doc_array.add(reminder_doc);
            }
        }
        return main_doc;

    }
    static JsonDocument unsimplify_Json(JsonDocument doc) {
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
    static JsonDocument simplify_box_Json(JsonDocument doc) {
        JsonDocument main_doc;
        const size_t doc_array_size = doc.size();
        if(doc_array_size==0) return JsonDocument();
        if(doc.as<JsonArray>().isNull()) {
            main_doc = simplify_box_Json_local(doc);
        }else {
            const auto main_doc_array= main_doc.to<JsonArray>();
            for(size_t i=0; i<doc_array_size; i++){
                JsonDocument box_doc=simplify_box_Json_local(doc[i]);
                main_doc_array.add(box_doc);
            }
        }
        return main_doc;
    }
    static JsonDocument unsimplify_box_Json(JsonDocument doc) {
        JsonDocument main_doc;
        const size_t doc_array_size = doc.size();
        if(doc_array_size==0) return JsonDocument();

        if(doc.as<JsonArray>().isNull()) {
            main_doc = unsimplify_box_Json_local(doc);
        }else {
            const auto main_doc_array= main_doc.to<JsonArray>();
            for(size_t i=0; i<doc_array_size; i++){
                JsonDocument box_doc = unsimplify_box_Json_local(doc[i]);
                main_doc_array.add(box_doc);
            }
        }
        return main_doc;
    }
    static bool validate_box_Json(JsonDocument doc){
        if(!doc["n"].is<String>()) return false;
        if(!doc["a"].is<uint16_t>()) return false;
        if(!doc["b"].is<uint8_t>()) return false;
        if( doc["b"].as<uint8_t>()==0) return false;
        if( doc.size()==0) return false;
        return true;
    }

    static bool check_box_json_equals(JsonDocument box_doc1, JsonDocument box_doc2) {
        if(box_doc1["a"].as<uint16_t>()!=box_doc2["a"].as<uint16_t>()) return false;
        if(box_doc1["n"].as<String>()!=box_doc2["n"].as<String>()) return false;
        if(box_doc1["b"].as<uint8_t>()!=box_doc2["b"].as<uint8_t>()) return false;
        return true;
    }




    static byte extractHour(const String &formated_time){
        return formated_time.substring(0, 2).toInt();
    }

    static byte extractMinute(const String &formated_time) {
        return formated_time.substring(3, 5).toInt();
    }
};
#endif //AV_FUNCTIONS_H
