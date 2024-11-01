//
// Created by user on 22/10/2024.
//

#ifndef NETWORK_COMMUNICATIONS_H
#define NETWORK_COMMUNICATIONS_H
#include <Arduino.h>
#include <WiFiClient.h>

class Network_communications {
    static void handle_client(WiFiClient client);

public:
    static void handle_network_comms();


    static bool initializeWiFi();
    static bool initializeMDNS();
    static bool initializeNTP();


    static bool resolve_WIFI_CONN_ERROR();
    static bool resolve_MDNS_ERROR();
    static bool resolve_BAD_WIFI_CRED();

    static IPAddress setAccessPoint();

    static IPAddress getAPIP();
    static IPAddress getWifiIP();
    static bool wifiConnected();

    static String handle_index_modeB(const String &remoteIp);
    static String handle_index(const String &remoteIp);

};



#endif //NETWORK_COMMUNICATIONS_H
