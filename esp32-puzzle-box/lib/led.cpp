#pragma once

#include <Adafruit_NeoPixel.h>
#include <esp_log.h>

#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20

class LEDStrip
{
public:
    LEDStrip(): led_strip(Adafruit_NeoPixel(NEOPIXEL_LED_COUNT, NEOPIXEL_LED_PIN, NEO_GRB + NEO_KHZ800))
    {
        ESP_LOGI("game1", "inside constructor: %d", led_strip.numPixels());
        led_strip.begin();
        led_strip.show();
        led_strip.setBrightness(10); // max 255
    }

    int numPixels() {
        return led_strip.numPixels();
    }

    void setPixelColor(uint16_t n, uint32_t c) {
        return led_strip.setPixelColor(n, c);
    }

    void show() {
        return led_strip.show();
    }

    void victory()
    {
        for (int times = 0; times < 5; times++)
        {
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, white);
            }
            led_strip.show();
            delay(100);
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, blue);
            }
            led_strip.show();
            delay(100);
        }
    }

    void failed()
    {
        for (int times = 0; times < 1; times++)
        {
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, white);
            }
            led_strip.show();
            delay(100);
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, red);
            }
            led_strip.show();
            delay(100);
        }
    }

    void clear()
    {
        for (int i = 0; i < led_strip.numPixels(); i++)
        {
            led_strip.setPixelColor(i, white);
        }
        led_strip.show();
    }

    uint32_t white = led_strip.Color(0, 0, 0);
    uint32_t red = led_strip.Color(255, 0, 0);
    uint32_t green = led_strip.Color(0, 255, 0);
    uint32_t blue = led_strip.Color(0, 0, 255);

private:
    Adafruit_NeoPixel led_strip;
};
