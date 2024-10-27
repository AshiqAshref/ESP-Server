//
// Created by user on 24/10/2024.
//

#ifndef ACCESS_POINT_H
#define ACCESS_POINT_H
#include <IPAddress.h>

class Access_point {
    bool AP_active=false;
    IPAddress IP;
public:
    Access_point()=default;
    bool is_active() const {return this->AP_active;}
    void set_active(const bool aP_status) {
        this-> AP_active=aP_status;
        if(!aP_status)
            this->IP = IPAddress();
    }

    void set_IP(const IPAddress &ip_address) {
        this->IP = ip_address;
        this->AP_active= true;
    }
};

#endif //ACCESS_POINT_H
