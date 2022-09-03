#include "Arduino.h"
#include <esp_log.h>
#include "Adafruit_VL53L0X.h"
#include "led.cpp"
#include "utils.cpp"

class Game2
{
public:
    explicit Game2(LEDStrip *led, Adafruit_VL53L0X &vl53lox) : led_strip(led), lox(vl53lox) {}

    void run()
    {
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

                led_strip->clear();
                int pixelGoalPost = floor(goalDepth / 25);
                int pixelCurrentDepth = floor(measure.RangeMilliMeter / 25);
                led_strip->setPixelColor(pixelGoalPost, led_strip->green);
                led_strip->setPixelColor(pixelCurrentDepth, led_strip->blue);
                led_strip->show();
                delay(100);
            }
            led_strip->victory();
        }
    }

private:
    LEDStrip *led_strip;
    Adafruit_VL53L0X lox;
};