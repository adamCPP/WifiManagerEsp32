#pragma once
#include <esp_http_server.h>
#include<optional>
#include<map>
#include<string>
#include <functional>
#include"esp_wifi_types.h" // for wifi_ap_record_t

class HttpServer
{
    httpd_handle_t server;
    httpd_uri_t uri_get;
    httpd_uri_t uri_post;
    httpd_uri_t androidCptv;
    httpd_uri_t microsiftCptv;
    httpd_uri_t uri_patch;
    httpd_uri_t uri_options;
    httpd_uri_t ws_uri_handler_options;

    // std::function<esp_err_t(httpd_req_t*)> handler;


public:
    // static std::optional<std::map<std::string, std:: string>> credentialsMap;
    HttpServer();
    void sendScanedAPs(std::vector<wifi_ap_record_t>);
    ~HttpServer();
    bool startServer(); 
    void stopServer();  // TODO consider moving it to private scope
};