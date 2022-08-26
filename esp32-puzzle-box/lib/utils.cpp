#pragma once 
#include <Adafruit_NeoPixel.h>

void victoryAnimation(Adafruit_NeoPixel &led)
{
    uint32_t white = led.Color(0, 0, 0);
    uint32_t red = led.Color(255, 0, 0);
    uint32_t green = led.Color(0, 255, 0);
    uint32_t blue = led.Color(0, 0, 255);

    for (int times = 0; times < 5; times++)
    {
        for (int i = 0; i < led.numPixels(); i++)
        {
            led.setPixelColor(i, white);
        }
        led.show();
        delay(100);
        for (int i = 0; i < led.numPixels(); i++)
        {
            led.setPixelColor(i, blue);
        }
        led.show();
        delay(100);
    }
}

void failedAnimation(Adafruit_NeoPixel &led)
{
    uint32_t white = led.Color(0, 0, 0);
    uint32_t red = led.Color(255, 0, 0);
    uint32_t green = led.Color(0, 255, 0);
    uint32_t blue = led.Color(0, 0, 255);

    for (int times = 0; times < 1; times++)
    {
        for (int i = 0; i < led.numPixels(); i++)
        {
            led.setPixelColor(i, white);
        }
        led.show();
        delay(100);
        for (int i = 0; i < led.numPixels(); i++)
        {
            led.setPixelColor(i, red);
        }
        led.show();
        delay(100);
    }
}

void clearDisplay(Adafruit_NeoPixel &led)
{
    uint32_t white = led.Color(0, 0, 0);
    for (int i = 0; i < led.numPixels(); i++)
    {
        led.setPixelColor(i, white);
    }
    led.show();
}