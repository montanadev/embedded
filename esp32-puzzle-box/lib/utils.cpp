#pragma once
#include <Adafruit_NeoPixel.h>


std::string urlDecode(const std::string &value)
{
    std::string result;
    result.reserve(value.size());

    for (std::size_t i = 0; i < value.size(); ++i)
    {
        auto ch = value[i];

        if (ch == '%' && (i + 2) < value.size())
        {
            auto hex = value.substr(i + 1, 2);
            auto dec = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            result.push_back(dec);
            i += 2;
        }
        else if (ch == '+')
        {
            result.push_back(' ');
        }
        else
        {
            result.push_back(ch);
        }
    }

    return result;
}

int getUrlParam(char *key, const char *parameter, char *value)
{
    char *addr1 = strstr(parameter, key);
    if (addr1 == NULL)
        return 0;
    
    char *addr2 = addr1 + strlen(key);
    
    char *addr3 = strstr(addr2, "&");
    if (addr3 == NULL)
    {
        strcpy(value, addr2);
    }
    else
    {
        int length = addr3 - addr2;
        strncpy(value, addr2, length);
        value[length] = 0;
    }
    return strlen(value);
}
