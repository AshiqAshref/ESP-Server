#ifndef COMMAND_H
#define COMMAND_H
#include <Arduino.h>

enum CommandStatus:byte {
    IN_PROGRESS = 0b10101010,
    FAILED = 0b01010101,
    COMPLETED = 0b00111100,
};

enum COMM_PROTOCOL:byte {
    SYN =             0b10000000,
    SYN_ACK =         0b01000000,
    ACK =             0b11000000,
    FIN =             0b00100000,
    UNKW_ERR=         0b01010000,
    RETRY =           0b01100000,
    SUCCESS=          0b01110000,
    TIMEOUT=          0b10100000,
    PROTOCOL_FILTER = 0b11110000
};

enum Command_enum:byte {
    GET_TIME =          0b00001000,
    GET_REMINDER_B =    0b00001100,
    ACTIVATE_AP =       0b00000100,
    DEACTIVATE_AP =     0b00000010,
    COMMAND_FILTER =    0b00001111
};

class Command {
protected:
    const Command_enum command_;
    CommandStatus status_ = COMPLETED;
    void (*send_request_)();
    bool (*response_handler_)();
    bool (*request_handler_)();

public:
    unsigned long last_millis=0;
    unsigned long retry_interval;

    Command(const Command_enum command, void(*send_request_)(), bool(*response_handler)(), bool(*request_handler)(),
            const unsigned long retry_interval)
        : command_(command),
          send_request_(send_request_),
          response_handler_(response_handler),
          request_handler_(request_handler),
          retry_interval(retry_interval) {
    }

protected:
    ~Command()= default;

public:
    virtual void send_request(){
		if(command_==GET_TIME)
			Serial.print("get_time ");
		if(command_ == GET_REMINDER_B) {
        	Serial.print("GET_REMB ");
        }if(command_ == ACTIVATE_AP) {
            Serial.print("ACT_AP ");
        }if(command_ == DEACTIVATE_AP) {
            Serial.print("DCT_AP ");
        }
        Serial.println("STARTING SND_REQ");
        this->set_status(IN_PROGRESS);
        this->send_request_();
    }

    virtual void set_status(const CommandStatus status) {
        this->status_=status;
        this->last_millis = millis();
    }

    void request_handler() const {
        Serial.println("STARTING REQ_H");
        this->request_handler_();
    }
    void response_handler() {
        Serial.println("STARTING RES_H");
        this->response_handler_()?
            this->set_status(COMPLETED):
            this->set_status(FAILED);
    }

    virtual Command_enum command()=0;
    CommandStatus status() const {return status_;}
};



#endif //COMMAND_H
