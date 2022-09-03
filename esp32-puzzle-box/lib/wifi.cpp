#include <esp_log.h>
#include "nvs_flash.h"
#include "nvs.h"
#include <WiFi.h>

bool isWifiStationMode(char *ssid, char *password)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("isWifiStationMode", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return false;
    }

    int32_t i = 0;
    err = nvs_get_i32(commitHandler, "sta_mode", &i);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        // nothing has been written, still in AP mode
        return false;
    }
    if (err != ESP_OK)
    {
        ESP_LOGI("isWifiStationMode", "Error (%s) reading from NVS", esp_err_to_name(err));
        return false;
    }
    size_t size = 60;
    err = nvs_get_str(commitHandler, "sta_ssid", ssid, &size);
    if (err != ESP_OK)
    {
        ESP_LOGI("isWifiStationMode", "Error (%s) reading ssid from NVS", esp_err_to_name(err));
        return false;
    }
    size = 60;
    err = nvs_get_str(commitHandler, "sta_password", password, &size);
    if (err != ESP_OK)
    {
        ESP_LOGI("isWifiStationMode", "Error (%s) reading password from NVS", esp_err_to_name(err));
        return false;
    }

    return i == 1;
}

void setWifiStationMode(char *ssid, char *password)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("setWifiStationMode", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return;
    }

    if (ssid && password && !ssid[0] && !password[0])
    {
        // reset back to AP mode
        err = nvs_set_i32(commitHandler, "sta_mode", 0);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
            return;
        }
        // erase ssid values
        nvs_erase_key(commitHandler, "sta_ssid");
        nvs_erase_key(commitHandler, "sta_password");
    }
    else
    {
        // write ssid values
        err = nvs_set_i32(commitHandler, "sta_mode", 1);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
            return;
        }
        err = nvs_set_str(commitHandler, "sta_ssid", ssid);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
            return;
        }
        err = nvs_set_str(commitHandler, "sta_password", password);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
            return;
        }
    }
    // finally, commit
    err = nvs_commit(commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("setWifiStationMode", "Error (%s) closing NVS handle", esp_err_to_name(err));
        return;
    }
    nvs_close(commitHandler);
}

bool wifiStartInStationMode()
{
    char ssid[60];
    char password[60];
    int totalAttempts = 0;
    if (isWifiStationMode(ssid, password))
    {
        while (totalAttempts < 5)
        {
            int connectionAttempts = 0;
            WiFi.mode(WIFI_STA);
            WiFi.begin(ssid, password);
            ESP_LOGI("app_main", "Connecting to wifi with %s and %s...\n", ssid, password);
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
        WiFi.softAP("your-name-with-dashes", "nathan-evans");
        IPAddress IP = WiFi.softAPIP();
        ESP_LOGI("app_main", "Soft AP IP address: %s", IP.toString().c_str());
    }
    return false;
}