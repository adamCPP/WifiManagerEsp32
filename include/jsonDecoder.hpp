#pragma once
#include<string>
#include<map>
#include <optional>

class JsonDecoder
{
public:
    static std::optional<std::map<std::string, std:: string>>  decodeJsonCredentials(std::string jsonContent);
    static std::string encodeJsonCredentials(std::map<std::string, std:: string> credentialsMap);
};