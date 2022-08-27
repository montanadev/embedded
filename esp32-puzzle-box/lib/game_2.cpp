#include "Arduino.h"
#include <esp_log.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"
#include "utils.cpp"

class Game2
{
public:
    Game2(Adafruit_NeoPixel &led, Adafruit_VL53L0X &vl53lox)
    {
        led_strip = led;
        lox = vl53lox;
    }

    void run()
    {
        uint32_t white = led_strip.Color(0, 0, 0);
        uint32_t red = led_strip.Color(255, 0, 0);
        uint32_t green = led_strip.Color(0, 255, 0);
        uint32_t blue = led_strip.Color(0, 0, 255);

        for (int games = 0; games < 3; games++)
        {
            int goalDepth = random(0, 300);
            int padding = 20;
            while (1)
            {
                VL53L0X_RangingMeasurementData_t measure;
                lox.rangingTest(&measure, false);

                if ((goalDepth - padding) < measure.RangeMilliMeter && measure.RangeMilliMeter < (goalDepth + padding))
                {
                    // within area, win!
                    break;
                }

                clearDisplay(led_strip);
                int pixelGoalPost = floor(goalDepth / 25);
                int pixelCurrentDepth = floor(measure.RangeMilliMeter / 25);
                led_strip.setPixelColor(pixelGoalPost, green);
                led_strip.setPixelColor(pixelCurrentDepth, blue);
                led_strip.show();
                delay(100);
            }
            victoryAnimation(led_strip);
        }
    }

private:
    Adafruit_NeoPixel led_strip;
    Adafruit_VL53L0X lox;
};