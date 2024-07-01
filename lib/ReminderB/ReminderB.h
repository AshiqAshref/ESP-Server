#ifndef REMINDERB_H
#define REMINDERB_H
#include <RTClib.h>
#include <Arduino.h>
#include <Box.h>


class ReminderB{
    int id=0;
    DateTime Upc = DateTime(0, 0, 0);
    Box boxes[];
    String boxes_{};
    byte boxes_size=0;
    boolean success=false;

public:
    template<size_t size>
    ReminderB(const DateTime &Upc, Box (&boxes)[size], boolean success);
    template<size_t size>
    ReminderB(int id, const DateTime &Upc, Box (&boxes)[size], boolean success);
    ReminderB(const DateTime &Upc, const String &boxes_, boolean success);
    ReminderB()=default;
    ~ReminderB() = default;

    [[nodiscard]] int & get_id();
    [[nodiscard]] byte & get_boxes_size();
    [[nodiscard]] boolean & get_success();
    [[nodiscard]] DateTime & get_date_time();
    [[nodiscard]] String toString() const;

    void set_id(int id);
    void set_upc(const DateTime &upc);
    void set_boxes_size(byte boxes_size);
    void set_success(boolean success);
};
#endif //REMINDERB_H
