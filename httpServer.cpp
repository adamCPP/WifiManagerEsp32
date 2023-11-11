#include"httpServer.hpp"
#include"indexHtml.h"
#include<esp_log.h>
#include "esp_spiffs.h"
#include<memory>
#include"jsonDecoder.hpp"
#include<map>
#include"customEvents.hpp"
// #include"wifiManagerIdf.hpp"


static const char *TAG = "HTTPServer";
// std::optional<std::map<std::string, std:: string>> HttpServer::credentialsMap= {};

// std::optional<std::map<std::string, std::string>> WifiManagerIdf::credentials_opt;


static esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG,"GET");
    const char* resp_str = html_page;
    
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG,"POST");

    std::unique_ptr<char[]> charArray = std::make_unique<char[]>(51);  // Allocate space for 50 characters + null terminator
    httpd_req_recv(req,charArray.get(),50);
    ESP_LOGI(TAG,"%s",charArray.get());
    std::optional<std::map<std::string, std:: string>>* credentials = new std::optional<std::map<std::string, std:: string>>();
    *credentials = JsonDecoder::decodeJsonCredentials(charArray.get());
    ESP_ERROR_CHECK(esp_event_post(CUSTOM_EVENTS, CREDENTIALS_AQUIRED, credentials, sizeof(*credentials),portMAX_DELAY));
    httpd_resp_send(req,"",HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t cors_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG,"OPTIONS cors handler");
    httpd_resp_set_hdr(req,"Access-Control-Allow-Headers","*");
    httpd_resp_set_hdr(req,"Access-Control-Allow-Origin","*");
    httpd_resp_set_hdr(req,"Access-Control-Allow-Methods","*");
    httpd_resp_send(req,"",HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


HttpServer::HttpServer()
{
    uri_get = {};
    uri_get.uri = "/";
    uri_get.method = HTTP_GET;
    uri_get.user_ctx = nullptr;
    uri_get.handler = get_handler;

    androidCptv = {};
    androidCptv.uri = "/generate_204";
    androidCptv.method = HTTP_GET;
    androidCptv.user_ctx = nullptr;
    androidCptv.handler = get_handler;

    microsiftCptv = {};
    microsiftCptv.uri = "/redirect";
    microsiftCptv.method = HTTP_GET;
    microsiftCptv.user_ctx = nullptr;
    microsiftCptv.handler = get_handler;
    

    uri_options = {};
    uri_options.uri = "/postCredentials";
    uri_options.method = HTTP_POST;
    uri_options.user_ctx = nullptr;
    uri_options.handler = post_handler;


    uri_post = {};
    uri_post.uri = "/postCredentials";
    uri_post.method = HTTP_OPTIONS;
    uri_post.user_ctx = nullptr;
    uri_post.handler = cors_handler;

    uri_patch= {};
    uri_patch.uri = "/postCredentials";
    uri_patch.method = HTTP_PATCH;
    uri_patch.user_ctx = nullptr;
    uri_patch.handler = cors_handler;


}

HttpServer::~HttpServer()
{
    stopServer();
}

bool HttpServer::startServer()
{

    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    /* Empty handle to esp_http_server */
    server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
        httpd_register_uri_handler(server, &uri_options);
        httpd_register_uri_handler(server, &androidCptv);
    }
    /* If server failed to start, handle will be NULL */
    return server == nullptr ? false:true;

}

void HttpServer::stopServer()
{
    if (server)
        {
            httpd_stop(server);
        }
}