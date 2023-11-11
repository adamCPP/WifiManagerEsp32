#include"jsonDecoder.hpp"
#include <cJSON.h>


std::optional<std::map<std::string, std:: string>> JsonDecoder::decodeJsonCredentials(std::string jsonContent)
{

    std::map<std::string, std::string> results;
    
        // Parse JSON response
    cJSON* root = cJSON_Parse(jsonContent.c_str());
    if (root == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error before: %s\n", error_ptr);
        }
        cJSON_Delete(root);  // Cleanup cJSON
        // Handle parsing error
        return std::nullopt;
    }

    // Extract data from JSON
    cJSON* ssid = cJSON_GetObjectItemCaseSensitive(root, "ssid");
    if (cJSON_IsString(ssid) && (ssid->valuestring != NULL)) {
        printf("ssid: %s\n", ssid->valuestring);
        results.insert(std::make_pair<std::string,std::string>("ssid",ssid->valuestring));
    }

    cJSON* password = cJSON_GetObjectItemCaseSensitive(root, "password");
    if (cJSON_IsString(password)) {
        printf("password: %s\n", password->valuestring);
        results.insert(std::make_pair<std::string,std::string>("password",password->valuestring));
    }

    cJSON_Delete(root);  // Cleanup cJSON
    return results;
}

std::string JsonDecoder::encodeJsonCredentials(std::map<std::string, std:: string> p_credentialsMap)
{
    cJSON* root = cJSON_CreateObject();

    const std::string ssid = p_credentialsMap["ssid"];
    const std::string password = p_credentialsMap["password"];



    cJSON_AddStringToObject(root, "ssid", ssid.c_str());
    cJSON_AddStringToObject(root, "password", password.c_str());

    char* jsonString = cJSON_Print(root);
    std::string encodedJson(jsonString);

    cJSON_free(jsonString);
    cJSON_Delete(root);

    return encodedJson;
}