#ifndef NETWORK_COMMUNICATIONS_H
#define NETWORK_COMMUNICATIONS_H
#include <ESPAsyncWebServer.h>


class Network_communications {
    static bool server_conn_test_local();


public:
    static void handle_network_comms();

    static bool get_revision_number();
    static bool get_reminder_B();

    static bool initializeWiFi();
    static bool initializeMDNS();
    static bool initializeNTP();

    static void initialize_self_server();
    static void initialize_AP_server();

    static bool resolve_WIFI_CONN_ERROR();
    static bool resolve_MDNS_ERROR();
    static bool resolve_BAD_WIFI_CRED();

    static bool handle_data();
    static bool server_conn_test();

    static IPAddress setAccessPoint();

    static IPAddress getAPIP();
    static IPAddress getWifiIP();
    static bool wifiConnected();
    static void req_toString(AsyncWebServerRequest *request);
    static void connection_end_protocol(const IPAddress &ip);





};



#endif //NETWORK_COMMUNICATIONS_H
