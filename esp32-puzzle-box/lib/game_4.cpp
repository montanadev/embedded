#include "Arduino.h"
#include <esp_log.h>
#include <Adafruit_NeoPixel.h>
#include "utils.cpp"

#define SOUND_PIN 34

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

class Game4
{
public:
    Game4()
    {
    }

    void run()
    {
        while (1)
        {

            unsigned long startMillis = millis(); // Start of sample window
            unsigned int peakToPeak = 0;          // peak-to-peak level

            unsigned int signalMax = 0;
            unsigned int signalMin = 1024;

            // collect data for 50 mS
            while (millis() - startMillis < sampleWindow)
            {
                sample = analogRead(SOUND_PIN);
                if (sample < 1024) // toss out spurious readings
                {
                    if (sample > signalMax)
                    {
                        signalMax = sample; // save just the max levels
                    }
                    else if (sample < signalMin)
                    {
                        signalMin = sample; // save just the min levels
                    }
                }
            }
            peakToPeak = signalMax - signalMin;       // max - min = peak-peak amplitude
            double volts = (peakToPeak * 5.0) / 1024; // convert to volts

            ESP_LOGI("game4", "volts=%f", volts);
            delay(100);
        }
    }
};