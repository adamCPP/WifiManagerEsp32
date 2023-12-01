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
    

    bool startHttpServer();
    void stopHttpServer();
    void reqisterCutomEvents();
    bool tryFetchCredentialsFromSPIFFS();
    
    wifi_config_t ap_config;
    wifi_config_t sta_config;
    wifi_scan_config_t scan_config;
    
    

public:
    std::optional<std::map<std::string, std::string>> credentials_opt;
    std::optional<bool> staStarted_opt;
    const WifiManagerIdfConfig managerConfig;
    std::vector<wifi_ap_record_t> foundedAPs;

    
    WifiManagerIdf(WifiManagerIdfConfig p_managerConfig);
    void sendScannedAP();
    bool setupWiFi(bool keepAP, bool andRun);
    void setupServerAndDns();
    void initWifi();
    void scanAvailableWifiNetworks();
    ~WifiManagerIdf();
    

};