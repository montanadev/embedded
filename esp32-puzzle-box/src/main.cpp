#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <WiFi.h>
#include <esp_http_server.h>
#include <esp_log.h>
#include "time.h"
#include <esp_system.h>
#include <vector>
#include <sys/param.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "../lib/game_1.cpp"
#include "../lib/game_2.cpp"
#include "../lib/game_3.cpp"
#include "../lib/game_4.cpp"
#include "../lib/web.cpp"
#include "../lib/wifi.cpp"

extern "C"
{
#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20

    // ntp
    const char *ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 3600;
    const int daylightOffset_sec = 3600;

    // Adafruit 7seg
    Adafruit_7segment matrix = Adafruit_7segment();
    // Adafruit_MPU6050 xyz gyro
    Adafruit_L3GD20_Unified mpu = Adafruit_L3GD20_Unified(20);
    // Adafruit_VL53L0X depth sensor
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();
    // LEDStrip 
    LEDStrip* led_strip = new LEDStrip();

    void showClock()
    {
        bool blink = false;
        while (1)
        {
            
            struct tm timeinfo;
            if (!getLocalTime(&timeinfo))
            {
                ESP_LOGI("showClock", "Failed to get time");
                delay(1000);
                continue;
            }
            int displayValue = timeinfo.tm_hour * 100 + timeinfo.tm_min;
            if (timeinfo.tm_hour == 0)
            {
                displayValue += 1200;
            }
            matrix.print(displayValue, DEC);

            for (int i = 0; i < 60; i++) {
                blink = !blink;
                matrix.drawColon(blink);
                matrix.writeDisplay();
                delay(1000);
            }
        }
    }

    // cppcheck-suppress unusedFunction
    void app_main()
    {
        initArduino();

        // esp-idf GPIO read logs are extremely noisy -- disable
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

        // initialize wifi
        wifiStart();

        // initalize ntp
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

        // start the webserver
        webserverStart();
        
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

        // for (uint16_t counter = 0; counter < 99; counter++)
        // {
        //     matrix.println(counter);
        //     matrix.writeDisplay();
        //     delay(10);
        // }

        // matrix.writeDisplay();

        showClock();
        led_strip->clear();

        // initialize games
        Game1 g1 = Game1(led_strip);
        Game2 g2 = Game2(led_strip, lox);
        Game3 g3 = Game3(led_strip, mpu);
        Game4 g4 = Game4();

        int i = 0;
        // play!
        matrix.println(i++);
        matrix.writeDisplay();
        // game1 is special: if input isn't detected, put into wall clock mode
        if (!g1.run(stationMode))
        {
            showClock();
        }
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