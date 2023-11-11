#pragma once
#include <esp_http_server.h>
#include<optional>
#include<map>
#include<string>

class HttpServer
{
    httpd_handle_t server;
    httpd_uri_t uri_get;
    httpd_uri_t uri_post;
    httpd_uri_t androidCptv;
    httpd_uri_t microsiftCptv;
    httpd_uri_t uri_patch;
    httpd_uri_t uri_options;


public:
    // static std::optional<std::map<std::string, std:: string>> credentialsMap;
    HttpServer();
    ~HttpServer();
    bool startServer(); 
    void stopServer();  // TODO consider moving it to private scope
};