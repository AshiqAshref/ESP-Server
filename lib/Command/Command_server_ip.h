#ifndef COMMAND_SERVER_IP_H
#define COMMAND_SERVER_IP_H

#define RES_GET true
#define RES_SET false
#include <Command.h>
#include <Output.h>

class Command_server_ip final : public Command {
    bool command_type_ = RES_GET;
    IPAddress server_ip_;
    bool (*response_handler_bool)(const IPAddress&, bool);
    // bool server_ip_valid_=false;
public:
    Command_server_ip(void(*send_request)(),
        bool(*response_handler_bool)(const IPAddress&, bool),
        bool(*request_handler)(),
        const unsigned long retry_interval_on_fail
        ) : Command(
                SERVER_IP,
                send_request,
                []{return true;},
                request_handler,
                retry_interval_on_fail
                ),response_handler_bool(response_handler_bool) {}


    void set_server_ip(const IPAddress &server_ip) {
        Output::print("newip:");
        Output::println(server_ip.toString(),false);
        this->server_ip_=server_ip;
        // this->server_ip_valid_=true;
    }
    IPAddress server_ip(){return this->server_ip_;}
    String server_address() const {return "http://"+this->server_ip_.toString()+":8080/";}

    Command_enum command() override {return this->command_;}
    bool command_type() const {return this->command_type_;}
    void send_request(const IPAddress &server_ip, const bool command_type=RES_GET){
        this->command_type_=command_type;
        this->server_ip_=server_ip;

        this->set_status(IN_PROGRESS);
        this->send_request_();
    }

    void response_handler() override {
        if(this->response_handler_bool(this->server_ip_, this->command_type_)) {
            this->set_status(COMPLETED);
        }else
            this->set_status(FAILED);
    }
};



#endif //COMMAND_SERVER_IP_H
