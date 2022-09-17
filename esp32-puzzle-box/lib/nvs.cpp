#pragma once

#include <esp_log.h>
#include "nvs_flash.h"
#include "nvs.h"

void nvs_write_str(char *key, char *value)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("write_str", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return;
    }
    err = nvs_set_str(commitHandler, key, value);
    if (err != ESP_OK)
    {
        ESP_LOGI("write_str", "Error (%s) writing %s to NVS", esp_err_to_name(err), key);
        return;
    }
    err = nvs_commit(commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("write_str", "Error (%s) closing NVS handle", esp_err_to_name(err));
        return;
    }
    nvs_close(commitHandler);
}

void nvs_write_int(char *key, int value)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("write_i32", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return;
    }
    err = nvs_set_i32(commitHandler, key, value);
    if (err != ESP_OK)
    {
        ESP_LOGI("write_i32", "Error (%s) writing %s to NVS", esp_err_to_name(err), key);
        return;
    }
    err = nvs_commit(commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("write_i32", "Error (%s) closing NVS handle", esp_err_to_name(err));
        return;
    }
    nvs_close(commitHandler);
}

int nvs_read_int(char *key, int fallback)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_read_int", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return fallback;
    }
    int32_t i = 0;
    err = nvs_get_i32(commitHandler, key, &i);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGI("nvs_read_int", "Not found (%s) reading %s, returning fallback %d", esp_err_to_name(err), key, fallback);
        return fallback;
    }
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_read_int", "Error (%s) reading %s, returning fallback %d", esp_err_to_name(err), key, fallback);
        return fallback;
    }
    return i;
}

char *nvs_read_str(char *key, char *fallback)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_read_str", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return fallback;
    }
    size_t size = 60;
    char s[60];
    err = nvs_get_str(commitHandler, key, s, &size);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGI("nvs_read_str", "Not found (%s) reading %s, returning fallback %s", esp_err_to_name(err), key, fallback);
        return fallback;
    }
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_read_str", "Error (%s) reading %s, returning fallback %s", esp_err_to_name(err), key, fallback);
        return fallback;
    }
    return s;
}

void nvs_delete(char *key)
{
    nvs_handle_t commitHandler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_delete", "Error (%s) opening NVS handle", esp_err_to_name(err));
        return;
    }
    err = nvs_erase_key(commitHandler, key);
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_delete", "Error (%s) deleting %s", esp_err_to_name(err), key);
        return;
    }
    err = nvs_commit(commitHandler);
    if (err != ESP_OK)
    {
        ESP_LOGI("nvs_delete", "Error (%s) closing NVS handle", esp_err_to_name(err));
        return;
    }
    nvs_close(commitHandler);
}
