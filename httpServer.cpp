#include"httpServer.hpp"
#include"indexHtml.h"
#include "esp_spiffs.h"
#include"jsonDecoder.hpp"
#include"customEvents.hpp"

#include<esp_log.h>
#include<esp_event.h>
#include<map>
#include<memory>



static const char *TAG = "HTTPServer";


static esp_err_t get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG,"GET");
    const char* resp_str = html_page;
    
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    ESP_ERROR_CHECK(esp_event_post(CUSTOM_EVENTS,SCAN_AVAILABLE_APS,nullptr,0,portMAX_DELAY));
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


 esp_err_t ws_handler(httpd_req_t *req)
{
    if (req->method == HTTP_GET) {
    ESP_LOGI(TAG, "Handshake done, the new connection was opened");
    return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt = {};
    ws_pkt.type = HTTPD_WS_TYPE_TEXT; // necessary?
    auto ret = httpd_ws_recv_frame(req,&ws_pkt,0);

    auto buff = std::make_unique<u_int8_t>(ws_pkt.len);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    if (ws_pkt.len)
    {

        ws_pkt.payload = buff.get();
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }

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

  
    ws_uri_handler_options= {};
    ws_uri_handler_options.uri = "/ws";
    ws_uri_handler_options.method = HTTP_GET;
    ws_uri_handler_options.user_ctx = nullptr;
    ws_uri_handler_options.handler =  ws_handler;
    ws_uri_handler_options.is_websocket = true;
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
    server = nullptr;

    /* Start the httpd server */
    auto err = httpd_start(&server, &config);
    if (err == ESP_OK) {
        /* Register URI handlers */
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);
        httpd_register_uri_handler(server, &uri_options);
        httpd_register_uri_handler(server, &androidCptv);
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &ws_uri_handler_options));

        return true;
    }
    else{
        /* If server failed to start, handle will be NULL */
        ESP_LOGE(TAG, "Failed to start a server (%s)", esp_err_to_name(err));
        return false;
    }

}

void HttpServer::stopServer()
{
    if (server)
        {
            httpd_stop(server);
        }
}