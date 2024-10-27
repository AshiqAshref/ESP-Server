#ifndef COMMAND_GET_TIME_H
#define COMMAND_GET_TIME_H
#include <Command.h>


class Command_get_time final : public Command {
    const unsigned long retry_interval_onSucc_= 3600000;
    const unsigned long retry_interval_onFail_= 30000;
public:
    Command_get_time(const Command_enum command, void(*send_request)(), bool(*response_handler)(), bool(*request_handler)(),
                     const unsigned long retry_interval)
        : Command(command, send_request, response_handler, request_handler, retry_interval) {
    }
    void set_status(const CommandStatus status) override {
        this->status_=status;
        last_millis = millis();
        if(status_==COMPLETED) {
            retry_interval=retry_interval_onSucc_;
        }else
            retry_interval=retry_interval_onFail_;
    }
    Command_enum command() override {return this->command_;}
    void send_request() override {
        set_status(COMPLETED);
    }


};



#endif //COMMAND_GET_TIME_H
