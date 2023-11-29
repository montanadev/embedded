#pragma once

#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_http_server.h>
#include <sys/param.h>
#include "nvs.cpp"

#define UUID     "hello"
#define PASSWORD "worldasd"

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[] asm("_binary_index_html_end");

bool wifi_started = false;

struct ApCredentials {
    String ssid;
    String password;
};

void restartWithDelay(void *parameter)
{
    delay(5000);
    esp_restart();
}

ApCredentials *getSsidAndPassword() {
    return new ApCredentials{UUID, PASSWORD};
}

bool isWifiStationMode()
{
    int mode = nvs_read_int("sta_mode", 0);
    return mode == 1;
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
    return false;
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

static esp_err_t indexHandler(httpd_req_t *req)
{
    // cppcheck-suppress comparePointers
    const uint32_t root_len = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html_start, root_len);
    return ESP_OK;
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
        set_timezone(atoi(timezone));
    }
    httpd_resp_sendstr(req, "Rebooting...<br /><br /><i>Note: if I fail to connect to the WiFi, use reset in the settings to fix me</i>\n");

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
            .handler = indexHandler,
            .user_ctx = NULL};
    httpd_register_uri_handler(server, &webpage_uri);
    httpd_uri_t print_uri = {
            .uri = "/settings",
            .method = HTTP_POST,
            .handler = settingsHandler};
    httpd_register_uri_handler(server, &print_uri);
}
void showWifi(Adafruit_SSD1306 *display, ApCredentials *credentials) {
    ESP_LOGI("showWifi", "Start wifi draw");
    if (!wifi_started) {
        WiFi.softAP(credentials->ssid, credentials->password);
        webserverStart();
        wifi_started = true;
    }
    IPAddress IP = WiFi.softAPIP();

    display->clearDisplay();
    display->setTextColor(WHITE);
    display->setTextSize(2);
    display->setFont(NULL);
    display->setCursor(0, 0);
    display->println("WiFi");
    display->drawLine(0, 15, 180, 15, 1);
    display->setTextSize(1);
    display->setCursor(0, 25);
    display->print("SSID: ");
    display->print(credentials->ssid);
    display->setCursor(0, 35);
    display->print("Password: ");
    display->print(credentials->password);
    display->setCursor(0, 55);
    display->print("http://");
    display->print(IP.toString());
    display->display();

    vTaskDelay(10 / portTICK_PERIOD_MS);
}