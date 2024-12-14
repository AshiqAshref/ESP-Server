//
// Created by user on 11/12/2024.
//

#ifndef COMMAND_GET_BOX_INF_H
#define COMMAND_GET_BOX_INF_H
#include <Command.h>


class Command_get_box_inf final: public Command{
public:
    Command_get_box_inf(
            void(*send_request)(),
            bool(*response_handler)(),
            bool(*request_handler)(),
            const unsigned long retry_interval_on_fail)
        :
        Command(
            GET_BOX_INF,
            send_request,
            response_handler,
            request_handler,
            retry_interval_on_fail
        ){}
    Command_enum command() override {return this->command_;}


};



#endif //COMMAND_GET_BOX_INF_H
