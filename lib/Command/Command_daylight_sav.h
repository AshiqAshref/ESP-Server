#ifndef COMMAND_DAYLIGHT_SAV_H
#define COMMAND_DAYLIGHT_SAV_H

#define RES_GET true
#define RES_SET false

#include <Command.h>

class Command_daylight_sav final : public Command{
    bool daylight_sav =false;
    bool command_type_ = RES_GET;
    bool (*response_handler_bool)(bool,bool);
    bool (*request_handler_bool)(bool);

public:
    Command_daylight_sav(void(*send_request)(),
        bool (*response_handler_bool)(bool,bool),
        bool(*request_handler_bool)(bool),
        const unsigned long retry_interval_on_fail
        ) : Command(
                DAYLIGHT_SAV,
                send_request,
                []{return true;},
                []{return true;},
                retry_interval_on_fail
                ),response_handler_bool(response_handler_bool), request_handler_bool(request_handler_bool)
    {
        this->Command::set_status(IN_PROGRESS);
    }

    Command_enum command() override {return this->command_;}
    bool command_type() const {return this->command_type_;}
    void send_request(const bool daylight_saving, const bool command_type=RES_GET){
        this->command_type_=command_type;
        this->daylight_sav=daylight_saving;
        this->set_status(IN_PROGRESS);
        this->send_request_();
    }

    void response_handler() override {
        if(this->response_handler_bool(this->daylight_sav, this->command_type_)) {
            this->set_status(COMPLETED);
        }else
            this->set_status(FAILED);
    }

    void request_handler() override {
        this->request_handler_bool(this->daylight_sav);
    }
};



#endif //COMMAND_DAYLIGHT_SAV_H
