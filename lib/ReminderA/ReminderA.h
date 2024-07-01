#ifndef REMINDERA_H
#define REMINDERA_H
#include <Arduino.h>
#include <RTClib.h>

class ReminderA{
    DateTime Upc=DateTime(0,0,0);
    byte boxNo = 0;
    unsigned int id = 0;
    boolean success = false;
public:
    ReminderA(const DateTime& Upc, byte boxNo, unsigned int id,boolean success);
    ReminderA() = default;
    ~ReminderA() = default;
    String toString() const;

    void set_id(unsigned int id);
    void set_box_no(byte box_no);
    void set_date_time(const DateTime &upc);
    void set_success(boolean success);

    unsigned int & get_id();
    byte & get_box_no();
    DateTime & get_date_time();
    boolean & get_success();
};

#endif //REMINDERA_H
