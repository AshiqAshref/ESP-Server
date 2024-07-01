#include "ReminderB.h"

template<size_t size>
ReminderB::ReminderB(const DateTime &Upc, Box (&boxes)[size], const boolean success) {
    this->Upc = Upc;
    this->boxes=boxes;
    this->success = success;
}

template<size_t size>
ReminderB::ReminderB(const int id, const DateTime &Upc, Box (&boxes)[size], const boolean success) {
    this->id = id;
    this->Upc = Upc;
    this->boxes=boxes;
    this->success = success;
}
ReminderB::ReminderB(const DateTime &Upc, const String &boxes_, const boolean success) {
    this->Upc =Upc;
    this->boxes_ = boxes_;
    this->success = success;
}


int & ReminderB::get_id() {return id;}
byte & ReminderB::get_boxes_size() {return boxes_size;}
boolean & ReminderB::get_success() {return success;}
DateTime & ReminderB::get_date_time() {return Upc;}

void ReminderB::set_id(const int id) {this->id = id;}
void ReminderB::set_upc(const DateTime &upc) {Upc = upc;}
void ReminderB::set_boxes_size(const byte boxes_size) {this->boxes_size = boxes_size;}
void ReminderB::set_success(const boolean success) {this->success = success;}

// Box ReminderB::printArray() const {return boxes;}
String ReminderB::toString() const {
    String time = "";
    if (Upc.hour() <= 9) {
        time += '0' + String(Upc.hour());
    } else {
        time += String(Upc.hour());
    }
    time += ':';
    if (Upc.minute() <= 9) {
        time += '0' + String(Upc.minute());
    } else {
        time += String(Upc.minute());
    }
    time += "";
    String a = "{"
                    R"(  'time': ")" + time +"\""+
                    R"(, 'boxes':)" + boxes+
                    R"(, 'success':)" + success +
                "}";
    return a;
}
