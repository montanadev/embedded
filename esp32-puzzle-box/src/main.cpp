#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <esp_log.h>

extern "C"
{
#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20
#define BUTTON_PIN 8

    Adafruit_NeoPixel led_strip(NEOPIXEL_LED_COUNT, NEOPIXEL_LED_PIN, NEO_GRB + NEO_KHZ800);
    uint32_t red = led_strip.Color(255, 0, 0);
    uint32_t green = led_strip.Color(0, 255, 0);
    uint32_t blue = led_strip.Color(0, 0, 255);

    void app_main()
    {
        ESP_LOGI("app_main", "Startup...");
        initArduino();

ESP_LOGI("app_main", "Startup...done0");
        // initialize neopixels
        led_strip.begin();
        ESP_LOGI("app_main", "Startup...done0.1");
        led_strip.show();
        ESP_LOGI("app_main", "Startup...done0.2");
        led_strip.setBrightness(50); // max 255
          ESP_LOGI("app_main", "Startup...done");

ESP_LOGI("app_main", "Startup...done1");

        // initialize button
        //pinMode(BUTTON_PIN, INPUT);
ESP_LOGI("app_main", "Startup...done2");

        for (int i = 0; i < led_strip.numPixels(); i++)
        {
            led_strip.setPixelColor(i, red);
            led_strip.show();
            delay(1000);
        }
        ESP_LOGI("app_main", "Startup...done3");


        while (1)
        {
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, green);
                led_strip.show();
                delay(1000);
            }
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, blue);
                led_strip.show();
                delay(1000);
            }
            ESP_LOGI("app_main", "Startup...done4");

        }
    }
}