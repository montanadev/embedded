#pragma once

#include <esp_log.h>
#include "nvs.cpp"
#include <WiFi.h>

bool isWifiStationMode()
{
    int mode = nvs_read_int("sta_mode", 0);
    return mode == 1;
}

void setWifiStationMode(char *ssid, char *password)
{
    if (ssid && password && !ssid[0] && !password[0])
    {
        // given empty input, back to AP mode
        nvs_write_int("sta_mode", 0);
        nvs_delete("sta_ssid");
        nvs_delete("sta_password");
    }
    else
    {
        // write ssid values
        nvs_write_int("sta_mode", 1);
        nvs_write_str("sta_ssid", ssid);
        nvs_write_str("sta_password", password);
    }
}

bool wifiStartInStationMode()
{
    int totalAttempts = 0;
    if (isWifiStationMode())
    {
        ESP_LOGI("wifiStartInStationMode", "Reading ssid...");
        String ssid = nvs_read_str("sta_ssid", "");
        String password = nvs_read_str("sta_password", "");
        while (totalAttempts < 5)
        {
            ESP_LOGI("wifiStartInStationMode", "ssid=%s password=%s", ssid.c_str(), password.c_str());
            int connectionAttempts = 0;
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid.c_str(), password.c_str());
            ESP_LOGI("app_main", "Connecting to wifi with %s and %s...\n", ssid.c_str(), password.c_str());
            while (WiFi.status() != WL_CONNECTED && connectionAttempts < 7)
            {
                connectionAttempts++;
                ESP_LOGI("app_main", ".\n");
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
            if (WiFi.status() == WL_CONNECTED)
            {
                ESP_LOGI("connect_wifi", "Connected @ %s\n", WiFi.localIP().toString().c_str());
                return true;
            }
            totalAttempts++;
        }
    }
    else
    {
        // initialize soft AP
        WiFi.softAP("your-name-with-dashes", "nathan-evans");
        IPAddress IP = WiFi.softAPIP();
        ESP_LOGI("app_main", "Soft AP IP address: %s", IP.toString().c_str());
    }
    // if there are many attempts made at the station, its likely the settings are bad
    if (totalAttempts == 5)
    {
        ESP_LOGI("app_main", "Failed to connext in station mode, reverting to soft AP mode");
        WiFi.softAP("your-name-with-dashes", "rebecca-ysteboe");
        IPAddress IP = WiFi.softAPIP();
        ESP_LOGI("app_main", "Soft AP IP address: %s", IP.toString().c_str());
    }
    return false;
}