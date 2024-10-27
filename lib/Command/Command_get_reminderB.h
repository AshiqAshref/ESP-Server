#ifndef COMMAND_GET_REMINDERB_H
#define COMMAND_GET_REMINDERB_H
#include <Command.h>

class Command_get_reminderB final : public Command {
    unsigned long next_reminder_time_key_=0;

public:
    Command_get_reminderB(const Command_enum command, void(*send_request)(), bool(*response_handler)(),
                          bool(*request_handler)(), const unsigned long retry_interval_on_fail)
        : Command(command, send_request, response_handler, request_handler, retry_interval_on_fail) {
    }
    unsigned long next_reminder_time_key() const {return this->next_reminder_time_key_;}
    void set_next_reminder_time_key(const unsigned long next_reminder_time_key_) {this->next_reminder_time_key_=next_reminder_time_key_;}
    void send_request(const unsigned long next_reminder_time_key){
        this->next_reminder_time_key_ = next_reminder_time_key;
        this->set_status(IN_PROGRESS);
        this->send_request_();
    }
    Command_enum command() override {return this->command_;}
    void send_request() override {
        set_status(COMPLETED);
    }


};



#endif //COMMAND_GET_REMINDERB_H
