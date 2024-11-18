#ifndef NET_RESOURCE_H
#define NET_RESOURCE_H
#include <Arduino.h>
#include <Output.h>

enum ReqStatus:byte {
    NET_FAILED =                0b01010101,
    NET_COMPLETED =             0b00111100,
    NET_COMPLETED_REFRESH =     0b10100101,
};

class Net_resource {
    ReqStatus status_ = NET_COMPLETED;

protected:
    bool (*start_request_)();
    ~Net_resource()=default;

    bool set_status_base(const ReqStatus status_) {
        this->status_=status_;
        this->last_millis = millis();
        if(status_==NET_COMPLETED || status_==NET_COMPLETED_REFRESH) {
            Output::draw_server_icon(true);
            return true;
        }
        Output::draw_server_icon(false);
        return false;
    }

    virtual bool set_status(const ReqStatus status) {
        return set_status_base(status);
    }

public:
    uint32_t retry_interval_;
    unsigned long last_millis=0;

    Net_resource(
            bool(*start_request)(),
            const uint32_t retry_interval
        ):
        start_request_(start_request),
        retry_interval_(retry_interval)
    {}

    virtual bool start_request() {
        if(this->start_request_())
            return this->set_status(NET_COMPLETED);
        return this->set_status(NET_FAILED);
    }


    ReqStatus status() const {return status_;}



};



#endif //NET_RESOURCE_H
