#include "Arduino.h"
#include <esp_log.h>
#include "utils.cpp"
#include "led.cpp"
#include "button.cpp"

class Game1
{
public:
    explicit Game1(LEDStrip *led) : led_strip(led) {}

    bool run()
    {
        for (int games = 0; games < 3; games++)
        {
            led_strip->clear();

            int goalLed = random(0, 20);
            bool playing = true;
            int failures = 0;
            while (playing)
            {
                led_strip->setPixelColor(goalLed, led_strip->green);
                for (int i = 0; i < led_strip->numPixels(); i++)
                {
                    int prev = i - 1;
                    if (i == 0)
                    {
                        prev = led_strip->numPixels() - 1;
                    }
                    // erase behind the current pixel, if its not the goal
                    if (prev != goalLed)
                    {
                        led_strip->setPixelColor(prev, led_strip->white);
                    }

                    if (i != goalLed)
                    {
                        led_strip->setPixelColor(i, led_strip->blue);
                    }

                    led_strip->show();
                    if (readButton())
                    {
                        ESP_LOGI("game1", "i=%d goalLed=%d", i, goalLed);
                        if (i == goalLed)
                        {
                            playing = false;
                            led_strip->victory();
                            break;
                        }
                        else
                        {
                            led_strip->failed();
                            led_strip->clear();
                            led_strip->setPixelColor(goalLed, led_strip->green);
                            led_strip->show();

                            // prevent player from holding the button
                            failures++;
                        }
                    }
                    
                    if (games == 0)
                    {
                        delay(100);
                    }
                    if (games == 1)
                    {
                        delay(50);
                    }
                    if (games == 2)
                    {
                        delay(30);
                    }
                    if (failures > 3)
                    {
                        playing = false;
                        break;
                    }
                }
            }
            if (failures > 3)
            {
                games = -1;
            }
        }
        return true;
    }

private:
    LEDStrip *led_strip;
};