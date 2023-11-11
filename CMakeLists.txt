idf_component_register(SRCS "SPIFFSControler.cpp" "jsonDecoder.cpp" "wifiManagerIdf.cpp" "dnsRedirector.c" "httpServer.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_server esp_netif esp_wifi spiffs json)
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
