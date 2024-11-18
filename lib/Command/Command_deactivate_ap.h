#ifndef COMMAND_DEACTIVATE_AP_H
#define COMMAND_DEACTIVATE_AP_H
#include <Command.h>


class Command_deactivate_ap final : public Command {
public:
    virtual ~Command_deactivate_ap() = default;
    Command_deactivate_ap(
            void(*send_request)(),
            bool(*response_handler)(),
            bool(*request_handler)(),
            const unsigned long retry_interval_on_fail
        ):Command(
            DEACTIVATE_AP,
            send_request,
            response_handler,
            request_handler,
            retry_interval_on_fail
        )
    {}
    Command_enum command() override {return this->command_;}

};



#endif //COMMAND_DEACTIVATE_AP_H
