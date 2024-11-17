#ifndef COMMAND_REMINDERB_CHANGE_H
#define COMMAND_REMINDERB_CHANGE_H
#include <Command.h>


class Command_reminderB_change final: public Command {
    unsigned long current_revision_no_=0;

public:
    Command_reminderB_change(void(*send_request)(), bool(*response_handler)(),
                              bool(*request_handler)(),
                              const unsigned long retry_interval_on_fail)
            : Command(REMINDERB_CH, send_request, response_handler, request_handler, retry_interval_on_fail)
   {    }

    unsigned long current_revision_no() const {return this->current_revision_no_;}
    void set_next_reminder_time_key(const unsigned long current_revision_no_) {this->current_revision_no_=current_revision_no_;}


    Command_enum command() override {return this->command_;}



};


#endif //COMMAND_REMINDERB_CHANGE_H
