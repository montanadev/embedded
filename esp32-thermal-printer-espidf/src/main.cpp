#include <WiFi.h>
#include "Arduino.h"
#include "Adafruit_Thermal.h"
#include "secrets.h"
#include "../lib/renderer/src/renderer.h"

#include "esp_https_ota.h"
#include "esp_http_client.h"
#include <esp_http_server.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_tls_crypto.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string>
#include "cJSON.h"

extern "C"
{

// Serial2 on esp32 is TX2/RX2 pins
Adafruit_Thermal printer(&Serial2);

void connect_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_name, ssid_password);
    ESP_LOGI("connect_wifi", "Connecting to wifi...\n");
    while (WiFi.status() != WL_CONNECTED) {
        ESP_LOGI("connect_wifi", ".\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI("connect_wifi", "Connected @ %s\n", WiFi.localIP().toString().c_str());
}

static esp_err_t pingHandler(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "result", "success");
    const char *response = cJSON_Print(root);
    httpd_resp_sendstr(req, response);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
                     evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD("_http_event_handler", "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

static esp_err_t otaHandler(httpd_req_t *req) {
    esp_http_client_config_t config = {
            .url = firmware_url,
            .event_handler = _http_event_handler,
            .keep_alive_enable = true,

    };

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        ESP_LOGI("otaHandler", "OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE("otaHandler", "Firmware upgrade failed");
    }

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

void callPrinterFunc(vector<string> command) {
    if (command[0] == "setFont") {
        // B, A
        printer.setFont(command[1].c_str()[0]);
    } else if (command[0] == "setSize") {
        // S, M, L
        printer.setSize(command[1].c_str()[0]);
    } else if (command[0] == "println") {
        printer.println(command[1].c_str());
    } else if (command[0] == "justify") {
        // L, C, R
        printer.justify(command[1].c_str()[0]);
    } else if (command[0] == "setLineHeight") {
        printer.setLineHeight(stoi(command[1]));
    } else if (command[0] == "feed") {
        printer.feed(stoi(command[1]));
    } else if (command[0] == "feedRows") {
        printer.feedRows(stoi(command[1]));
    } else if (command[0] == "inverseOn") {
        printer.inverseOn();
    } else if (command[0] == "inverseOff") {
        printer.inverseOff();
    } else if (command[0] == "boldOn") {
        printer.boldOn();
    } else if (command[0] == "boldOff") {
        printer.boldOff();
    } else if (command[0] == "underlineOn") {
        printer.underlineOn();
    } else if (command[0] == "underlineOff") {
        printer.underlineOff();
    } else if (command[0] == "doubleHeightOn") {
        printer.doubleHeightOn();
    } else if (command[0] == "doubleHeightOff") {
        printer.doubleHeightOff();
    } else {
        ESP_LOGI("callPrinterFunc", "%s not recognized", command[0].c_str());
    }
}

static esp_err_t printHandler(httpd_req_t *req) {
    char buf[1000];
    int ret, remaining = req->content_len;
    int immutableRemaining = remaining;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;
    }

    std::string data;
    for (int i = 0; i < immutableRemaining; i++) {
        data += buf[i];
    }

    if (data.find("Content-Disposition: form-data") != std::string::npos) {
        // found a file upload
        auto lines = splitToLines(data);
        string shrank;

        // ignore the first 3 lines (content type / header info), and the last 1 line
        for (int i = 0; i < lines.size(); i++) {
            if (i <= 2 || i == lines.size() - 1) {
                continue;
            }
            shrank += lines[i] + '\n';
        }
        data = shrank;
    }

    auto commands = renderer(data);
    ESP_LOGI("printHandler", "Got commands:");
    for (auto command: commands) {
        for (auto subCommand: command) {
            ESP_LOGI("printHandler", "command: %s", subCommand.c_str());
        }
    }

    for (auto command: commands) {
        for (auto subCommand: command) {
            ESP_LOGI("printHandler", "running command: %s", subCommand.c_str());
        }
        callPrinterFunc(command);
    }

    // give a good chunk of paper to easily pull off the receipt
    printer.feed(4);
    printer.sleep();

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

void app_main() {
    initArduino();

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI("app_main", "Starting connect to wifi...");
    connect_wifi();
    ESP_LOGI("app_main", "Starting connect to wifi...done");

    ESP_LOGI("app_main", "Starting printer...");
    Serial2.begin(19200);
    printer.begin();
    ESP_LOGI("app_main", "Starting printer...done");

    // httpd task is the only thing running, take as much of the heap (minus padding) as possible
    int stack_size = xPortGetFreeHeapSize() - 1000;
    ESP_LOGI("app_main", "allocating %d to httpd tasks", stack_size);

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.stack_size = stack_size;
    httpd_start(&server, &config);

    // test handlers, and potentially monitoring urls
    httpd_uri_t ping_uri = {
            .uri = "/ping",
            .method = HTTP_GET,
            .handler = pingHandler};
    httpd_register_uri_handler(server, &ping_uri);

    // POST markdown url
    httpd_uri_t print_uri = {
            .uri = "/print",
            .method = HTTP_POST,
            .handler = printHandler};
    httpd_register_uri_handler(server, &print_uri);

    // OTA handler
    httpd_uri_t ota_uri = {
            .uri = "/ota",
            .method = HTTP_POST,
            .handler = otaHandler};
    httpd_register_uri_handler(server, &ota_uri);

    ESP_LOGI("app_main", "running v%d", 2);
}
}