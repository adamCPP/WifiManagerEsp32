#pragma once
#include<memory>
#include"httpServer.hpp"
#include "esp_wifi.h"
#include<memory>
#include<vector>
#include <functional>

struct WifiManagerIdfConfig
{
    std::string ssid = "WIFI universal manager";
    bool shouldKeepAP = true;
    std::map<std::string,std::string> customParametersMap;
    std::function<void(void)> customParametersReceivedCallback;
    bool enableLogger = true;
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
    WifiManagerIdfConfig managerConfig;
    std::vector<wifi_ap_record_t> foundedAPs;

    
    WifiManagerIdf(const WifiManagerIdfConfig& p_managerConfig);
    void sendLog(std::string log);
    void sendScannedAP();
    void sendCustomParameters();
    bool setupWiFi(bool keepAP, bool andRun);
    void setupServerAndDns();
    void initWifi();
    void scanAvailableWifiNetworks();
    ~WifiManagerIdf();
    

};