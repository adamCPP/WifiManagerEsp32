#pragma once
#include<memory>
#include"httpServer.hpp"
#include "esp_wifi.h"
#include<memory>


class WifiManagerIdf
{

    std::unique_ptr<HttpServer> httpServer_ptr;
    

    void setupAP(bool andRun);
    bool startHttpServer();
    void stopHttpServer();
    void reqisterCutomEvents();
    bool tryFetchCredentialsFromSPIFFS();
    
    wifi_config_t ap_config;
    wifi_config_t sta_config;

public:
    const bool PRESISTENT_AP;
    std::optional<std::map<std::string, std::string>> credentials_opt;
    std::optional<bool> staStarted_opt;

    
    WifiManagerIdf(bool should_keep_ap = true);
    bool setupWiFi(bool keepAP, bool andRun);
    void setupAPwithServer(bool andRun);
    void initWifi();
    ~WifiManagerIdf();
    

};