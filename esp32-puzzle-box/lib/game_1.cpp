#include "Arduino.h"
#include <esp_log.h>
#include "utils.cpp"
#include "led.cpp"

#define BUTTON_PIN 4
#define BUTTON_LED 2

class Game1
{
public:
    explicit Game1(LEDStrip *led) : led_strip(led) {}

    bool readButton()
    {
        int buttonState = digitalRead(BUTTON_PIN);
        return buttonState == HIGH;
    }

    bool run(bool quitIfNotPlaying)
    {
        
        ESP_LOGI("game_1", "Starting game1");
        for (int games = 0; games < 3; games++)
        {
            // initialize game
            this->led_strip->clear();

            int goalLed = random(0, 20);

            bool playing = true;
            int failures = 0;
            int buttonPresses = 0;
            int frame = 0;
            while (playing)
            {
                    ESP_LOGI("game1", "a");
                this->led_strip->setPixelColor(goalLed, this->led_strip->green);
                ESP_LOGI("game1", "b: pixel %d", this->led_strip->numPixels());
                for (int i = 0; i < this->led_strip->numPixels(); i++)
                {
                    ESP_LOGI("game1", "c");
                    frame++;
                    int prev = i - 1;
                    if (i == 0)
                    {
                        prev = this->led_strip->numPixels() - 1;
                    }
                    ESP_LOGI("game1", "d");
                    // erase behind the current pixel, if its not the goal
                    if (prev != goalLed)
                    {
                        this->led_strip->setPixelColor(prev, this->led_strip->white);
                    }

                    if (i != goalLed)
                    {
                        this->led_strip->setPixelColor(i, this->led_strip->blue);
                    }
                    ESP_LOGI("game1", "e");
                    this->led_strip->show();
                    ESP_LOGI("game1", "trying to read button");
                    if (readButton())
                    {
                        ESP_LOGI("game1", "i=%d goalLed=%d", i, goalLed);
                        buttonPresses++;
                        if (i == goalLed)
                        {
                            playing = false;
                            this->led_strip->victory();
                            break;
                        }
                        else
                        {
                            this->led_strip->failed();
                            this->led_strip->clear();
                            this->led_strip->setPixelColor(goalLed, this->led_strip->green);
                            this->led_strip->show();

                            // prevent player from holding the button
                            failures++;
                        }
                    }
                    else
                    {
                        ESP_LOGI("game1", "button not pressed");
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
                    if (failures > 3)
                    {
                        playing = false;
                        break;
                    }
                    if (frame > 50 && buttonPresses == 0 && quitIfNotPlaying)
                    {
                        // user probably isn't playing,
                        playing = false;
                        break;
                    }
                }
            }
            if (failures > 3)
            {
                games = -1;
            }
            if (frame > 50 && buttonPresses == 0 && quitIfNotPlaying)
            {
                return false;
            }
        }
        return true;
    }

private:
    LEDStrip *led_strip;
};