#include "Arduino.h"
#include <esp_log.h>
#include <Adafruit_NeoPixel.h>
#include "utils.cpp"

#define BUTTON_PIN 13
#define BUTTON_LED 2

class Game1
{
public:
    Game1(Adafruit_NeoPixel &led)
    {
        led_strip = led;
    }

    bool readButton()
    {
        int buttonState = digitalRead(BUTTON_PIN);
        return buttonState == HIGH;
    }

    void run()
    {
        uint32_t white = led_strip.Color(0, 0, 0);
        uint32_t red = led_strip.Color(255, 0, 0);
        uint32_t green = led_strip.Color(0, 255, 0);
        uint32_t blue = led_strip.Color(0, 0, 255);
        
        for (int games = 0; games < 3; games++)
        {
            // initialize game
            clearDisplay(led_strip);

            int goalLed = random(0, 20);

            bool playing = true;
            int failures = 0;
            while (playing)
            {
                led_strip.setPixelColor(goalLed, green);
                for (int i = 0; i < led_strip.numPixels(); i++)
                {
                    int prev = i - 1;
                    if (i == 0)
                    {
                        prev = led_strip.numPixels() - 1;
                    }
                    // erase behind the current pixel, if its not the goal
                    if (prev != goalLed)
                    {
                        led_strip.setPixelColor(prev, white);
                    }

                    if (i != goalLed)
                    {
                        led_strip.setPixelColor(i, blue);
                    }

                    led_strip.show();
                    if (readButton())
                    {
                        ESP_LOGI("game1", "i=%d goalLed=%d", i, goalLed);
                        if (i == goalLed)
                        {
                            playing = false;
                            victoryAnimation(led_strip);
                            break;
                        }
                        else
                        {
                            failedAnimation(led_strip);
                            clearDisplay(led_strip);
                            led_strip.setPixelColor(goalLed, green);
                            led_strip.show();

                            // prevent player from holding the button
                            failures++;    
                        }
                    }
                    // TODO - move the delay into the button and offer more checks
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
                    if (failures > 3) {
                        playing = false;
                        break;
                    }
                }
            }
            if (failures > 3) {
                games = -1;
            }
        }
    }

private:
    Adafruit_NeoPixel led_strip;
};