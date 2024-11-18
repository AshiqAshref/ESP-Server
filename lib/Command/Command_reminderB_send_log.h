#ifndef COMMAND_REMINDERB_SEND_LOG_H
#define COMMAND_REMINDERB_SEND_LOG_H
#include <Command.h>


class Command_reminderB_send_log final:public Command{
public:
    Command_reminderB_send_log(
            void(*send_request_)(),
            bool(*response_handler_)(),
            bool(*request_handler_)(),
            const uint32_t retry_interval_onFail
        ):Command(
            REMINDERB_SND_LOG,
            send_request_,
            response_handler_,
            request_handler_,
            retry_interval_onFail
        )
    {}

    Command_enum command() override {return this->command_;}
};



#endif //COMMAND_REMINDERB_SEND_LOG_H
