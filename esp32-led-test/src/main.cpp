#include "Arduino.h"

#include <Adafruit_NeoPixel.h>
#include "../lib/led.cpp"

extern "C"
{
#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20

    void asdf(LEDStrip *s)
    {
        s->failed();
        delay(1000);
        s->failed();
        delay(1000);
    }

    void app_main(void)
    {
        initArduino();

        LEDStrip *led_strip = new LEDStrip();

        led_strip->victory();
        delay(1000);
        led_strip->victory();
        delay(1000);
        led_strip->failed();
        delay(1000);
        asdf(led_strip);
    }
}