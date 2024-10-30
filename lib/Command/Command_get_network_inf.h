#ifndef COMMAND_GET_NETWORK_INF_H
#define COMMAND_GET_NETWORK_INF_H
#include <Command.h>


class Command_get_network_inf final :public Command {
public:
    Command_get_network_inf(void(*send_request)(), bool(*response_handler)(),
                                  bool(*request_handler)(), const unsigned long retry_interval_on_fail)
                : Command(GET_NETWORK_INF, send_request, response_handler, request_handler, retry_interval_on_fail) {
    }
    Command_enum command() override {return this->command_;}

};



#endif //COMMAND_GET_NETWORK_INF_H
