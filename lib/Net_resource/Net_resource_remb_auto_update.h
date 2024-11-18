

#ifndef NET_RESOURCE_REMB_AUTO_UPDATE_H
#define NET_RESOURCE_REMB_AUTO_UPDATE_H
#include <Net_resource.h>
#include <Net_resource_get_reminder_B.h>
#include <Net_resource_get_rev_no_B.h>



class Net_resource_remb_auto_update final: public Net_resource{
    Net_resource_get_reminder_B &net_resource_get_reminder_B_;
    Net_resource_get_rev_no_B &net_resource_get_rev_no_B_;
    uint32_t(*memmory_get_revision_no_)();
    uint32_t retry_interval_onFail_;
    uint32_t retry_interval_onSucc_;

public:
    Net_resource_remb_auto_update(
            Net_resource_get_reminder_B &net_resource_get_reminder_B,
            Net_resource_get_rev_no_B &net_resource_get_rev_no_B,
            uint32_t(*memmory_get_revision_no_)(),
            const uint32_t retry_interval_onFail,
            const uint32_t retry_interval_onSucc
        ):
        Net_resource([]{return true;},retry_interval_onFail),
        net_resource_get_reminder_B_(net_resource_get_reminder_B),
        net_resource_get_rev_no_B_(net_resource_get_rev_no_B),
        memmory_get_revision_no_(memmory_get_revision_no_),
        retry_interval_onFail_(retry_interval_onFail),
        retry_interval_onSucc_(retry_interval_onSucc)
    {}

    bool start_request() override {
        if(this->net_resource_get_rev_no_B_.start_request()) {
            if(this->memmory_get_revision_no_()!=this->net_resource_get_rev_no_B_.revision_no())
                net_resource_get_reminder_B_.start_request();
            return this->set_status(NET_COMPLETED);
        }
        return this->set_status(NET_FAILED);
    }

     bool set_status(const ReqStatus status) override {
        if(status==NET_COMPLETED || status==NET_COMPLETED_REFRESH) {
            this->retry_interval_ = retry_interval_onSucc_;
            return set_status_base(NET_COMPLETED_REFRESH);
        }
        this->retry_interval_=retry_interval_onFail_;
        return set_status_base(status);
    }
};



#endif //NET_RESOURCE_REMB_AUTO_UPDATE_H


