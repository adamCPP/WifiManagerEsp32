#pragma once
#include<memory>
#include"httpServer.hpp"
#include "esp_wifi.h"
#include<memory>
#include<vector>

struct WifiManagerIdfConfig
{
    std::string ssid = "WIFI universal manager";
    bool shouldKeepAP = true;
};

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
    wifi_scan_config_t scan_config;
    std::vector<wifi_ap_record_t> foundedAPs;
    

public:
    std::optional<std::map<std::string, std::string>> credentials_opt;
    std::optional<bool> staStarted_opt;
    const WifiManagerIdfConfig managerConfig;

    
    WifiManagerIdf(WifiManagerIdfConfig p_managerConfig);
    bool setupWiFi(bool keepAP, bool andRun);
    void setupAPwithServer(bool andRun);
    void initWifi();
    void scanAvailableWifiNetworks();
    ~WifiManagerIdf();
    

};