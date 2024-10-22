//
// Created by user on 22/10/2024.
//

#ifndef NETWORK_COMMUNICATIONS_H
#define NETWORK_COMMUNICATIONS_H
#include <Arduino.h>

class Network_communications {
public:
    static bool initializeWiFi();
    static bool initializeMDNS();
    static bool initializeNTP();


    static bool resolve_WIFI_CONN_ERROR();
    static bool resolve_MDNS_ERROR();
    static bool resolve_BAD_WIFI_CRED();

    static IPAddress setAccessPoint();

    static String handle_index_modeB(const String &remoteIp);
    static String handle_index(const String &remoteIp);

};



#endif //NETWORK_COMMUNICATIONS_H
