#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <WiFi.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include <esp_system.h>
#include <vector>
#include <sys/param.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "../lib/game_1.cpp"
#include "../lib/game_2.cpp"
#include "../lib/game_3.cpp"
#include "../lib/game_4.cpp"

extern "C"
{
#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20

    extern const char index_html_start[] asm("_binary_index_html_start");
    extern const char index_html_end[] asm("_binary_index_html_end");

    // Adafruit 7seg
    Adafruit_7segment matrix = Adafruit_7segment();
    // Adafruit_MPU6050 xyz gyro
    Adafruit_L3GD20_Unified mpu = Adafruit_L3GD20_Unified(20);
    // Adafruit_VL53L0X depth sensor
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();
    // Adafruit_NeoPixel
    Adafruit_NeoPixel led_strip(NEOPIXEL_LED_COUNT, NEOPIXEL_LED_PIN, NEO_GRB + NEO_KHZ800);
    uint32_t white = led_strip.Color(0, 0, 0);
    uint32_t red = led_strip.Color(255, 0, 0);
    uint32_t green = led_strip.Color(0, 255, 0);
    uint32_t blue = led_strip.Color(0, 0, 255);

    void game1()
    {
        Game1 g = Game1(led_strip);
        g.run();
    }

    void game2()
    {
        Game2 g = Game2(led_strip, lox);
        g.run();
    }

    void game3()
    {
        Game3 g = Game3(led_strip, mpu);
        g.run();
    }

    int findValue(char *key, const char *parameter, char *value)
    {
        char *addr1 = strstr(parameter, key);
        if (addr1 == NULL)
            return 0;
        ESP_LOGD("findValue", "addr1=%s", addr1);

        char *addr2 = addr1 + strlen(key);
        ESP_LOGD("findValue", "addr2=[%s]", addr2);

        char *addr3 = strstr(addr2, "&");
        ESP_LOGD("findValue", "addr3=%p", addr3);
        if (addr3 == NULL)
        {
            strcpy(value, addr2);
        }
        else
        {
            int length = addr3 - addr2;
            ESP_LOGD("findValue", "addr2=%p addr3=%p length=%d", addr2, addr3, length);
            strncpy(value, addr2, length);
            value[length] = 0;
        }
        ESP_LOGD("findValue", "key=[%s] value=[%s]", key, value);
        return strlen(value);
    }

    static esp_err_t indexHandler(httpd_req_t *req)
    {
        const uint32_t root_len = index_html_end - index_html_start;
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, index_html_start, root_len);
        return ESP_OK;
    }

    bool isWifiStationMode(char *ssid, char *password)
    {
        nvs_handle_t commitHandler;
        esp_err_t err = nvs_open("storage", NVS_READWRITE, &commitHandler);
        if (err != ESP_OK)
        {
            ESP_LOGI("isWifiStationMode", "Error (%s) opening NVS handle", esp_err_to_name(err));
            while (1)
                ;
        }

        int32_t i = 0;
        err = nvs_get_i32(commitHandler, "sta_mode", &i);
        if (err != ESP_OK)
        {
            ESP_LOGI("isWifiStationMode", "Error (%s) reading from NVS", esp_err_to_name(err));
            while (1)
                ;
        }
        err = nvs_get_str(commitHandler, "sta_ssid", ssid, 0);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) reading from NVS", esp_err_to_name(err));
            while (1)
                ;
        }
        err = nvs_get_str(commitHandler, "sta_password", password, 0);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) reading from NVS", esp_err_to_name(err));
            while (1)
                ;
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
            while (1)
                ;
        }

        if (ssid && password && !ssid[0] && !password[0])
        {
            // reset back to AP mode
            err = nvs_set_i32(commitHandler, "sta_mode", 0);
            if (err != ESP_OK)
            {
                ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
                while (1)
                    ;
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
                while (1)
                    ;
            }
            err = nvs_set_str(commitHandler, "sta_ssid", ssid);
            if (err != ESP_OK)
            {
                ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
                while (1)
                    ;
            }
            err = nvs_set_str(commitHandler, "sta_password", password);
            if (err != ESP_OK)
            {
                ESP_LOGI("setWifiStationMode", "Error (%s) writing to NVS", esp_err_to_name(err));
                while (1)
                    ;
            }
        }
        // finally, commit
        err = nvs_commit(commitHandler);
        if (err != ESP_OK)
        {
            ESP_LOGI("setWifiStationMode", "Error (%s) closing NVS handle", esp_err_to_name(err));
            while (1)
                ;
        }
        nvs_close(commitHandler);
    }

    static esp_err_t settingsHandler(httpd_req_t *req)
    {
        char buf[1000];
        int ret, remaining = req->content_len;
        int immutableRemaining = remaining;

        while (remaining > 0)
        {
            /* Read the data for the request */
            if ((ret = httpd_req_recv(req, buf,
                                      MIN(remaining, sizeof(buf)))) <= 0)
            {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                {
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
        for (int i = 0; i < immutableRemaining; i++)
        {
            data += buf[i];
        }
        ESP_LOGI("settingsHandler", "data: %s", data.c_str());

        char ssid[60];
        char password[60];
        findValue("ssid=", data.c_str(), ssid);
        findValue("password=", data.c_str(), password);

        setWifiStationMode(ssid, password);

        httpd_resp_send_chunk(req, NULL, 0);
        return ESP_OK;
    }

    void app_main()
    {
        initArduino();

        // for some reason, pin reads are extremely noisy -- disable
        esp_log_level_set("gpio", ESP_LOG_NONE);

        // initialize non-volatile storage
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            // NVS partition was truncated and needs to be erased
            // Retry nvs_flash_init
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);

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
                ESP_LOGI("app_main", "Connecting to wifi...\n");
                while (WiFi.status() != WL_CONNECTED && connectionAttempts < 7)
                {
                    connectionAttempts++;
                    ESP_LOGI("app_main", ".\n");
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
                if (WiFi.status() == WL_CONNECTED)
                {
                    ESP_LOGI("connect_wifi", "Connected @ %s\n", WiFi.localIP().toString().c_str());
                    break;
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
        if (totalAttempts == 5) {
            ESP_LOGI("app_main", "Failed to connext in station mode, reverting to soft AP mode");
            WiFi.softAP("your-name-with-dashes", "nathan-evans");
            IPAddress IP = WiFi.softAPIP();
            ESP_LOGI("app_main", "Soft AP IP address: %s", IP.toString().c_str());
        }

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

        // initialize neopixels
        ESP_LOGI("app_main", "Initializing strip...");
        led_strip.begin();
        led_strip.show();
        led_strip.setBrightness(10); // max 255
        ESP_LOGI("app_main", "Initializing strip...done");

        // initialize lox
        ESP_LOGI("app_main", "Initializing VL53L0X...");
        if (!lox.begin())
        {
            ESP_LOGI("app_main", "Failed to boot VL53L0X");
            while (1)
                ;
        }
        ESP_LOGI("app_main", "Initializing VL53L0X...done");

        // initialize gyro
        ESP_LOGI("app_main", "Initializing MPU6050...");
        if (!mpu.begin())
        {
            ESP_LOGI("app_main", "Failed to boot MPU6050");
            while (1)
                ;
        }
        ESP_LOGI("app_main", "Initializing MPU6050...done");

        // initialize button
        pinMode(BUTTON_LED, OUTPUT);
        pinMode(BUTTON_PIN, INPUT);

        // initialize motor
        // pinMode(MOTOR_PIN, OUTPUT);

        // initalize sound
        // pinMode(SOUND_PIN, INPUT);

        // initialize 7seg
        matrix.begin(0x70);
        matrix.setBrightness(1);

        for (uint16_t counter = 0; counter < 99; counter++)
        {
            matrix.println(counter);
            matrix.writeDisplay();
            delay(10);
        }

        matrix.writeDisplay();

        // initialize games
        Game1 g1 = Game1(led_strip);
        Game2 g2 = Game2(led_strip, lox);
        Game3 g3 = Game3(led_strip, mpu);
        Game4 g4 = Game4();

        int i = 0;
        // play!
        matrix.println(i++);
        matrix.writeDisplay();
        g1.run();
        matrix.println(i++);
        matrix.writeDisplay();
        g2.run();
        matrix.println(i++);
        matrix.writeDisplay();
        g3.run();
        matrix.println(i++);
        matrix.writeDisplay();
        g4.run();
    }
}