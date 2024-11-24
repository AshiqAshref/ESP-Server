#ifndef NET_RESOURCE_POST_REMB_STAT_H
#define NET_RESOURCE_POST_REMB_STAT_H
#include <Net_resource.h>

class Net_resource_post_remB_stat final:public Net_resource {
    JsonDocument remb_log_json{};
    bool (*start_request_json_)(JsonDocument);
public:
    Net_resource_post_remB_stat(
        bool(*start_request)(JsonDocument),
        const uint32_t retry_interval
        ):Net_resource(
            [](){return true;},
            retry_interval
        ),
        start_request_json_(start_request)
    {}

    void start_request(const JsonDocument &remb_log) {
        this->remb_log_json=remb_log;
        this->start_request();

    }

    bool start_request() override {
        if(this->start_request_json_(remb_log_json))
            return this->set_status(NET_COMPLETED);
        return this->set_status(NET_FAILED);
    }

};

#endif //NET_RESOURCE_POST_REMB_STAT_H
