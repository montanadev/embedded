#include <esp_http_server.h>
#include <sys/param.h>
#include <esp_log.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "utils.cpp"
#include "wifi.cpp"
#include "clock.cpp"

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[] asm("_binary_index_html_end");

static esp_err_t indexHandler(httpd_req_t *req)
{
    // cppcheck-suppress comparePointers
    const uint32_t root_len = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html_start, root_len);
    return ESP_OK;
}

void restartWithDelay(void *parameter)
{
    delay(5000);
    esp_restart();
}

static esp_err_t settingsHandler(httpd_req_t *req)
{
    char buf[1000];
    int remaining = req->content_len;
    int immutableRemaining = remaining;

    while (remaining > 0)
    {
        /* Read the data for the request */
        int ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
        if (ret <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    std::string data;
    for (int i = 0; i < immutableRemaining; i++)
    {
        data += buf[i];
    }
    data = urlDecode(data);
    // ESP_LOGD("settingsHandler", "data: %s", data.c_str());

    char ssid[60];
    char password[60];
    char timezone[12];
    if (getUrlParam("ssid=", data.c_str(), ssid) && getUrlParam("password=", data.c_str(), password))
    {
        setWifiStationMode(ssid, password);
    }
    if (getUrlParam("timezone=", data.c_str(), timezone))
    {
        setTimezone(atoi(timezone));
    }
    httpd_resp_sendstr(req, "Rebooting puzzle box...<br /><br /><i>Note: if I fail to connect to the WiFi, I'll eventually give up and create an AP (so you can reach these settings again)</i>\n");

    // defer the reboot, allow the current thread to return a HTTP response
    xTaskCreate(restartWithDelay, "restartWithDelay", 10000, NULL, 1, NULL);

    return ESP_OK;
}

void webserverStart()
{
    // initalize web server
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    httpd_start(&server, &config);

    // attach the landing page
    httpd_uri_t webpage_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = indexHandler};
    httpd_register_uri_handler(server, &webpage_uri);
    // attach the settings handler
    httpd_uri_t print_uri = {
        .uri = "/settings",
        .method = HTTP_POST,
        .handler = settingsHandler};
    httpd_register_uri_handler(server, &print_uri);
}
