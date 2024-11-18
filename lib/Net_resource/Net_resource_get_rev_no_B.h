#ifndef NET_RESOURCE_GET_REV_NO_B_H
#define NET_RESOURCE_GET_REV_NO_B_H
#include <Net_resource.h>


class Net_resource_get_rev_no_B final:public Net_resource {
    bool (*start_request_)(uint32_t&);
    uint32_t revision_no_=0;
public:
    Net_resource_get_rev_no_B(
        bool (*start_request_)(uint32_t&),
        const uint32_t retry_interval
        ):Net_resource(
            [](){return true;},
            retry_interval
        ),
        start_request_(start_request_)
    {}

    bool start_request() override {
        if(start_request_(revision_no_))
            return this->set_status(NET_COMPLETED);
        return this->set_status(NET_FAILED);
    }
    void set_revision_no(const uint32_t revision_no_) {
        this->revision_no_=revision_no_;
    }
    uint32_t revision_no() const {return this->revision_no_;}

};



#endif //NET_RESOURCE_GET_REV_NO_B_H
