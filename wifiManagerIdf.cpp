#include"wifiManagerIdf.hpp"
#include<stdio.h>
#include <cstring>
#include<esp_log.h>
#include<memory>
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/ip_addr.h"
#include"dnsRedirector.h"
#include"customEvents.hpp"
#include"spiffsControler.hpp"
#include<utility>
	

static const char *TAG = "WIFI Mgr";

void set_wifi_ap_ip(esp_netif_t *esp_netif_ap) 
{
    
    esp_netif_ip_info_t IP_settings_ap;

    IP_settings_ap.ip.addr=ipaddr_addr("4.3.2.1");
    IP_settings_ap.gw.addr=ipaddr_addr("4.3.2.1");
    IP_settings_ap.netmask.addr=ipaddr_addr("255.255.255.0");
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(esp_netif_ap));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &IP_settings_ap));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));

}

static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    auto wifiManagerIdf = static_cast<WifiManagerIdf*>(arg);

    if(!wifiManagerIdf)
    {
        ESP_LOGE(TAG,"[DEBUG]casting is not ok");
    }
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG,"WIFI_EVENT_AP_STACONNECTED");

    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG,"WIFI_EVENT_AP_STADISCONNECTED");

    }
    else if (event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG,"WIFI_EVENT_STA_START");
        esp_wifi_connect();

    }
    else if (event_id == WIFI_EVENT_STA_STOP) {
        ESP_LOGI(TAG,"WIFI_EVENT_STA_STOP");
        // esp_wifi_connect();

    }
    else if (event_id == WIFI_EVENT_AP_START) {
        ESP_LOGI(TAG,"WIFI_EVENT_AP_START");

    }
    else if (event_id == WIFI_EVENT_AP_STOP) {
        ESP_LOGI(TAG,"WIFI_EVENT_AP_STOP");

    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG,"WIFI_EVENT_STA_CONNECTED");
        if(!wifiManagerIdf->staStarted_opt.has_value())
        {
            wifiManagerIdf->staStarted_opt = true; // ToDo consider removing it
        }

    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* event = (wifi_event_sta_disconnected_t*) event_data; //TODO null check

        ESP_LOGI(TAG,"WIFI_EVENT_STA_DISCONNECTED reasonId %d",event->reason);
        if(event->reason == 201 ) ESP_LOGE(TAG, "Wrong SSID");
        if(event->reason == 202 ) ESP_LOGE(TAG, "Wrong Password");
        if(!wifiManagerIdf->staStarted_opt.has_value()) wifiManagerIdf->staStarted_opt = false;
        // ESP_ERROR_CHECK(esp_event_post(CUSTOM_EVENTS, CREDENTIALS_AQUIRED, nullptr, 0,portMAX_DELAY
        if(event->reason == 201 or event->reason == 202) // wong ssid or password
        {
            wifiManagerIdf->setupWiFi(true,true);
            wifiManagerIdf->setupServerAndDns();
        }
    }
    else if(event_id == WIFI_EVENT_SCAN_DONE)
    {
        ESP_LOGI(TAG,"WIFI_EVENT_SCAN_DONE");
        uint16_t ap_count = 0;
        esp_wifi_scan_get_ap_num(&ap_count);
        wifi_ap_record_t* accessPoints =  new wifi_ap_record_t[ap_count];
        esp_wifi_scan_get_ap_records(&ap_count,accessPoints);

        wifiManagerIdf->foundedAPs = std::vector<wifi_ap_record_t>(ap_count); //TODO  this is unnecessary since we have sending response immediately

        for(auto i = 0; i<ap_count;++i)
        {
            wifiManagerIdf->foundedAPs.push_back(accessPoints[i]);
            ESP_LOGI(TAG,"SSID: %s RSSI %d",(const char*)accessPoints[i].ssid,accessPoints[i].rssi);
        }
        wifiManagerIdf->sendScannedAP();
        delete[] accessPoints;
    }
    else
    {
        ESP_LOGI(TAG,"Other event. ID:%d",int(event_id));
    } 
}

static void customEventsHandler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{

    auto wifiMgrPtr = static_cast<WifiManagerIdf*>(arg);
    

    if (event_id == CREDENTIALS_AQUIRED)
    {   
        ESP_LOGI(TAG,"CREDENTIALS_AQUIRED event received");
        if(wifiMgrPtr)
        { 
            auto ptr= static_cast<std::optional<std::map<std::string, std:: string>>*>(event_data);
            if(ptr == nullptr)
            {
                ESP_LOGE(TAG,"Bad casting");
            }
            wifiMgrPtr->credentials_opt = ptr->value();
            if(wifiMgrPtr->managerConfig.shouldKeepAP) 
            {
                ESP_LOGI(TAG,"Starting AP_STA");
                wifiMgrPtr->setupWiFi(wifiMgrPtr->managerConfig.shouldKeepAP,true);
            }
            else{
                ESP_LOGI(TAG,"Starting only STA");
                 wifiMgrPtr->setupWiFi(wifiMgrPtr->managerConfig.shouldKeepAP,true); //TODO duplication
            }

            if(wifiMgrPtr->credentials_opt.has_value())
            {
                SPIFFSControler spiffsControler;
                spiffsControler.writeCredentials(wifiMgrPtr->credentials_opt.value());
            }
 
        }
    }
    else if (event_id == SCAN_AVAILABLE_APS)
    {
        ESP_LOGI(TAG,"SCAN_AVAILABLE_APS event received");
        wifiMgrPtr->scanAvailableWifiNetworks();
    }
    else{
        ESP_LOGE(TAG,"Undefined custom event received. Id %d",CREDENTIALS_AQUIRED);
    }
}

WifiManagerIdf::WifiManagerIdf(const WifiManagerIdfConfig p_managerConfig):
managerConfig(p_managerConfig)
{

    ap_config = {};
    strncpy((char*)ap_config.ap.ssid, managerConfig.ssid.c_str(), sizeof(ap_config.ap.ssid) - 1);
    ap_config.ap.ssid_len = managerConfig.ssid.length();
    ap_config.ap.max_connection = 1;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;

    sta_config = {};

    scan_config = {
    .ssid = 0,
    .bssid = 0,
    .channel = 0,
    .show_hidden = true
  };
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    reqisterCutomEvents();
    initWifi();
    esp_netif_t *esp_netif_ap = esp_netif_create_default_wifi_ap(); //TODO deinit this in destructor or something
    set_wifi_ap_ip(esp_netif_ap);

    bool credFetched = tryFetchCredentialsFromSPIFFS();
    if(credFetched)
    {
        setupWiFi(managerConfig.shouldKeepAP,true);
        if(managerConfig.shouldKeepAP) setupServerAndDns();
    }
    else{
        setupWiFi(true,true);
        setupServerAndDns();
    }

}

void WifiManagerIdf::setupServerAndDns()
{

    bool serverStarted = startHttpServer();
    if(serverStarted)
    {
        ESP_LOGI(TAG,"[DEBUG] Server started");
        start_dns_server();
    }
    else
    {
        ESP_LOGI(TAG,"[DEBUG] Server NOT started");
    }
}

void WifiManagerIdf::initWifi()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.ampdu_rx_enable = false; // no idea what i am doing here but this is for android captive portal
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    &wifiEventHandler,
                                                    this,
                                                    NULL));
}


void WifiManagerIdf::reqisterCutomEvents()
{
    
    ESP_ERROR_CHECK(esp_event_handler_register(CUSTOM_EVENTS,
                                                    ESP_EVENT_ANY_ID,
                                                    &customEventsHandler,
                                                    this));
    // ESP_ERROR_CHECK(esp_event_post(CUSTOM_EVENTS, CREDENTIALS_AQUIRED, nullptr, 0,portMAX_DELAY));


}


bool WifiManagerIdf::setupWiFi(bool keepAP, bool andRun)
{

    if(credentials_opt.has_value())
    {
        auto ssid = credentials_opt.value()["ssid"].c_str();
        auto password = credentials_opt.value()["password"].c_str();
        strncpy((char*)sta_config.sta.ssid, ssid, sizeof(sta_config.sta.ssid) - 1);
        strncpy((char*)sta_config.sta.password, password, sizeof(sta_config.sta.password) - 1);

        // esp_netif_create_default_wifi_sta(); // TODO deinit this in destructor or something
        ESP_ERROR_CHECK(esp_wifi_set_mode(keepAP ? WIFI_MODE_APSTA : WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
        if(keepAP) ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));  
        if(andRun) ESP_ERROR_CHECK(esp_wifi_start());      
    }
    else
    {
        ESP_LOGE(TAG,"Credentials for sta mod hasn't been set");
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
        if(keepAP) ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));  
        if(andRun) ESP_ERROR_CHECK(esp_wifi_start());  
        return false;
    }
    return true;
}


bool WifiManagerIdf::startHttpServer()
{
    if(httpServer_ptr)
    {
        ESP_LOGD(TAG,"HttpServer already started");
        return false;
    }

    httpServer_ptr = std::make_unique<HttpServer>();
    return httpServer_ptr->startServer();
}

void WifiManagerIdf::stopHttpServer()
{
    if(httpServer_ptr == nullptr)
    {
        ESP_LOGE(TAG,"HttpServer already stopped");
    }
    httpServer_ptr.reset(); // it calls HttpServer destructor 

}

bool WifiManagerIdf::tryFetchCredentialsFromSPIFFS()
{
    SPIFFSControler spiffsControler;


    credentials_opt = spiffsControler.readCredentials();

    if(credentials_opt.has_value() && credentials_opt.value().find("ssid") != credentials_opt.value().end() && !credentials_opt.value()["ssid"].empty())
    {

        ESP_LOGD(TAG, "Credentials readed");
        return true;  
    }
    else{

        ESP_LOGE(TAG, "Credentials NOT readed");
        
    }

    return false;
}

void WifiManagerIdf::scanAvailableWifiNetworks() // wifi has to be in sta mode
{

  ESP_LOGE(TAG, "Scanning available networks");
  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
}

void WifiManagerIdf::sendScannedAP()
{
    httpServer_ptr->sendScanedAPs(foundedAPs);
}

WifiManagerIdf::~WifiManagerIdf()
{
    stopHttpServer();
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_netif_deinit();
}