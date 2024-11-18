#ifndef COMMAND_REMINDERB_CHANGE_H
#define COMMAND_REMINDERB_CHANGE_H
#include <Command.h>


class Command_reminderB_change final: public Command {

public:
    Command_reminderB_change(
            void(*send_request)(),
            bool(*response_handler)(),
            bool(*request_handler)(),
            const uint32_t retry_interval_on_fail
        ):Command(
            REMINDERB_CH,
            send_request,
            response_handler,
            request_handler,
            retry_interval_on_fail
        )
   {}

    Command_enum command() override {return this->command_;}
};


#endif //COMMAND_REMINDERB_CHANGE_H
