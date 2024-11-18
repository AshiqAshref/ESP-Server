#ifndef NET_RESOURCE_GET_REMINDER_B_H
#define NET_RESOURCE_GET_REMINDER_B_H
#include <Net_resource.h>
#include <Net_resource_get_rev_no_B.h>
#include <Command_reminderB_change.h>



class Net_resource_get_reminder_B final:public Net_resource {
    bool (*start_request_)(uint32_t&);
    Net_resource_get_rev_no_B &net_resource_get_rev_no_B;
    Command_reminderB_change &command_reminderB_change_;

public:
    Net_resource_get_reminder_B(
            bool (*start_request_)(uint32_t&),
            Net_resource_get_rev_no_B& net_resource_get_rev_no_B,
            Command_reminderB_change &command_reminderB_change_,
            const uint32_t retry_interval
        ):
        Net_resource([]{return true;}, retry_interval),
        start_request_(start_request_),
        net_resource_get_rev_no_B(net_resource_get_rev_no_B),
        command_reminderB_change_(command_reminderB_change_)
    {}

    bool start_request() override {
        uint32_t reminder_no;
        if(this->start_request_(reminder_no)) {
            if(reminder_no!=net_resource_get_rev_no_B.revision_no()) {
                command_reminderB_change_.send_request();
            }
            net_resource_get_rev_no_B.set_revision_no(reminder_no);
            return this->set_status(NET_COMPLETED);
        }
        return this->set_status(NET_FAILED);
    }
};



#endif //NET_RESOURCE_GET_REMINDER_B_H
