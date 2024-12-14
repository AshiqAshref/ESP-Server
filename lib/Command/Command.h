#ifndef COMMAND_H
#define COMMAND_H
#include <Arduino.h>

enum CommandStatus:byte {
    IN_PROGRESS =           0b10101010,
    FAILED =                0b01010101,
    COMPLETED =             0b00111100,
    COMPLETED_REFRESH =     0b10100101,
};


enum COMM_PROTOCOL:byte {
    SYN =             0b10000000,
    SYN_ACK =         0b01000000,
    ACK =             0b11000000,
    READY_TO_SEND =   0b11100000,
    READY_TO_RECV =   0b00110000,
    FIN =             0b00100000,
    UNKW_ERR=         0b01010000,
    RETRY =           0b01100000,
    SUCCESS=          0b01110000,
    TIMEOUT=          0b10100000,
    PROTOCOL_FILTER = 0b11110000
};

enum Command_enum:byte {
    GET_TIME =          0b00000010,
    GET_NETWORK_INF =   0b00000011,
    ACTIVATE_AP =       0b00000100,
    DEACTIVATE_AP=      0b00000101,
    GET_BOX_INF =       0b00000110,
    REMINDERB_SND_LOG=  0b00001000,
    REMINDERB_CH =      0b00001001,
    GET_REMINDER_B =    0b00001011,
    DAYLIGHT_SAV =      0b00001100,
    SERVER_IP =         0b00001101,
    COMMAND_FILTER =    0b00001111
    // 0b00000001,
    // 0b00000111,
    // 0b00001010,
    // 0b00001110,
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
        this->set_status(IN_PROGRESS);
        this->send_request_();
    }

    virtual void set_status(const CommandStatus status) {
        this->status_=status;
        this->last_millis = millis();
    }

    virtual void request_handler() {
        this->request_handler_();
    }
    virtual void response_handler() {
        this->response_handler_()?
            this->set_status(COMPLETED):
            this->set_status(FAILED);
    }



    virtual Command_enum command()=0;
    CommandStatus status() const {return status_;}
};

/* *********************************************************************************
 *                              CREATING A COMMAND                                 *
 *  1) Add command to Command_enum and assign a bite to it                         *
 *  2) create a class and pass new command_enum in constructor                     *
 *  3) Add command_enum to Communication_protocols::command_as_String()            *
 *  4) create send_request, request_handler and response_handler for the command.  *
 *  5) declare and initialise the new Command class in main                        *
 *  6) extend the command declaration in CommunicationHandler                      *
 *  7) add command to CommunicationHandler::commands array.                        *
 ********************************************************************************* */

#endif //COMMAND_H
