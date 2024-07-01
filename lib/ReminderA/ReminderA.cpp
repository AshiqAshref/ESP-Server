#include "ReminderA.h"



ReminderA::ReminderA(const DateTime& Upc, const byte boxNo, const unsigned int id, const boolean success){
    this->Upc=Upc;
    this->boxNo=boxNo;
    this->id=id;
    this->success=success;
}

void ReminderA::set_id(const unsigned int id) {this->id = id;}
void ReminderA::set_box_no(const byte box_no) {boxNo = box_no;}
void ReminderA::set_date_time(const DateTime &upc) {Upc = upc;}
void ReminderA::set_success(const boolean success) {this->success = success;}

unsigned int & ReminderA::get_id() {return id;}
byte & ReminderA::get_box_no() {return boxNo;}
DateTime & ReminderA::get_date_time() {return Upc;}
boolean & ReminderA::get_success() {return success;}

String ReminderA::toString() const {
    String time="\"";
    String BoxNo="\"";
    if(Upc.hour()<=9){
        time+='0'+String(Upc.hour());
    }else{
        time+=String(Upc.hour());
    }
    time+=':';
    if(Upc.minute()<=9){
        time+='0'+String(Upc.minute());
    }else{
        time+=String(Upc.minute());
    }
    time+="\"";

    if(boxNo<=9){
        BoxNo+='0'+String(boxNo);
    }else{
        BoxNo+=String(boxNo);
    }
    BoxNo+="\"";

    String a="{"
             R"( 'id':)"        +String(id)+
             R"(,'date': ")"    +time+ "\""+
             R"(,'boxNo': )"    +BoxNo+
             R"(,'success':)"   +success+
        "}";
    return a;
}
