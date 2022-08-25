#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"
#include <Adafruit_MPU6050.h>
#include <esp_log.h>
#include <vector>

extern "C"
{
#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20
#define BUTTON_PIN 4
#define BUTTON_LED 2

    // Adafruit_MPU6050 xyz gyro
    Adafruit_MPU6050 mpu;
    // Adafruit_VL53L0X depth sensor
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();
    // Adafruit_NeoPixel
    Adafruit_NeoPixel led_strip(NEOPIXEL_LED_COUNT, NEOPIXEL_LED_PIN, NEO_GRB + NEO_KHZ800);
    uint32_t white = led_strip.Color(0, 0, 0);
    uint32_t red = led_strip.Color(255, 0, 0);
    uint32_t green = led_strip.Color(0, 255, 0);
    uint32_t blue = led_strip.Color(0, 0, 255);

    bool readButton()
    {
        int buttonState = digitalRead(BUTTON_PIN);
        return buttonState == HIGH;
    }

    void failedAnimation()
    {
        for (int times = 0; times < 1; times++)
        {
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, white);
                delay(1);
            }
            led_strip.show();
            delay(100);
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, red);
                delay(1);
            }
            led_strip.show();
            delay(100);
        }
    }

    void victoryAnimation()
    {
        for (int times = 0; times < 5; times++)
        {
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, white);
                delay(1);
            }
            led_strip.show();
            delay(100);
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, blue);
                delay(1);
            }
            led_strip.show();
            delay(100);
        }
    }

    void game1()
    {
        for (int games = 0; games < 3; games++)
        {
            // initialize game
            for (int i = 0; i < led_strip.numPixels(); i++)
            {
                led_strip.setPixelColor(i, white);
            }
            led_strip.show();

            int goalLed = random(0, 20);

            bool playing = true;
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
                            victoryAnimation();
                            break;
                        }
                        else
                        {
                            failedAnimation();
                            for (int i = 0; i < led_strip.numPixels(); i++)
                            {
                                led_strip.setPixelColor(i, white);
                            }
                            led_strip.setPixelColor(goalLed, green);
                            led_strip.show();
                        }
                    }
                    // TODO - move the delay into the button and offer more checks
                    if (games == 0)
                    {
                        delay(100);
                    }
                    if (games == 1)
                    {
                        delay(80);
                    }
                    if (games == 2)
                    {
                        delay(50);
                    }
                }
            }
        }
    }

    void game2()
    {
        for (int games = 0; games < 3; games++)
        {
            int goalDepth = random(0, 500);
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

                // reset pixels to white
                for (int i = 0; i < led_strip.numPixels(); i++)
                {
                    led_strip.setPixelColor(i, white);
                }
                int pixelGoalPost = floor(goalDepth / 25);
                int pixelCurrentDepth = floor(measure.RangeMilliMeter / 25);
                led_strip.setPixelColor(pixelGoalPost, green);
                led_strip.setPixelColor(pixelCurrentDepth, blue);
                led_strip.show();
                delay(100);
            }
            victoryAnimation();
        }
    }

    void clearDisplay()
    {
        for (int i = 0; i < led_strip.numPixels(); i++)
        {
            led_strip.setPixelColor(i, white);
        }
        led_strip.show();
    }

    // determineMovement takes a raw movement from the gyro, normalizes it against the
    // baseline for that axis (the mpu reports different baseline values for each axis)
    // and then cuts off at a given threshold, to remove noise
    // if movement is below threshold, 0 is returned
    int determineMovement(int movement, int baseline, int threshold)
    {
        if (movement < (baseline - threshold))
        {
            return abs(movement / 10);
        }
        if (movement > (baseline + threshold))
        {
            return -1 * abs(movement / 10);
        }
        return 0;
    }

    void displayRanges(int pos, std::vector<std::vector<int>> ranges)
    {
        for (auto range : ranges)
        {
            int start = range[0];
            int stop = range[1];
            int pin = range[2];
            if (pos < stop && pos > start)
            {
                led_strip.setPixelColor(pin, green);
                break;
            }
        }
        if (pos < ranges[0][0]) {
            led_strip.setPixelColor(ranges[0][2], green);
        }
        if (pos > ranges[ranges.size() - 1][1]) {
            led_strip.setPixelColor(ranges[ranges.size() - 1][2], green);
        }
    }

    int clamp(int i, int low, int high) {
        if (i < low) {
            return low;
        }
        if (i > high) {
            return high;
        }
        return i;
    }

    void game3()
    {
        int threshold = 400;
        int xPos = 0;
        int yPos = 0;
        int zPos = 0;

        clearDisplay();
        led_strip.setPixelColor(2, green);
        led_strip.show();

        while (1)
        {
            sensors_event_t a, g, temp;
            mpu.getEvent(&a, &g, &temp);

            int x = floor(g.gyro.x * 100); // x normalizes at -14
            int y = floor(g.gyro.y * 100); // y normalizes at 1
            int z = floor(g.gyro.z * 100); // z at 3

            // ESP_LOGI("game3", "Rotation X: %d, Y: %d, Z: %d", x, y, z);

            // TODO - determine what movement looks like
            xPos += clamp(determineMovement(x, -14, threshold), -250, 250);
            yPos += clamp(determineMovement(y, 1, threshold), -250, 250);
            zPos += clamp(determineMovement(z, 3, threshold), -250, 250);

            //ESP_LOGI("game3", "xpos=%d", xPos);
            //ESP_LOGI("game3", "ypos=%d", yPos);
            ESP_LOGI("game3", "zpos=%d z=%d", zPos, z);

            clearDisplay();
            displayRanges(
                xPos,
                {
                    {-250, -150, 0},
                    {-150, -50, 1},
                    {-50, 50, 2},
                    {50, 150, 3},
                    {150, 250, 4},
                });
            displayRanges(
                yPos,
                {
                    {-250, -150, 5},
                    {-150, -50, 6},
                    {-50, 50, 7},
                    {50, 150, 8},
                    {150, 250, 9},
                });
            displayRanges(
                zPos,
                {
                    {-250, -150, 10},
                    {-150, -50, 11},
                    {-50, 50, 12},
                    {50, 150, 13},
                    {150, 250, 14},
                });
            led_strip.show();
            
            delay(100);
        }
    }

    void app_main()
    {
        initArduino();

        // initialize neopixels
        ESP_LOGI("app_main", "Initializing strip...");
        led_strip.begin();
        led_strip.show();
        led_strip.setBrightness(10); // max 255
        ESP_LOGI("app_main", "Initializing strip...done");

        // initialize lox
        ESP_LOGI("app_main", "Initializing VL53L0X...");
        if (!lox.begin())
        {
            ESP_LOGI("app_main", "Failed to boot VL53L0X");
            while (1)
                ;
            ESP_LOGI("app_main", "Initializing VL53L0X...done");
        }

        // initialize gyro
        ESP_LOGI("app_main", "Initializing MPU6050...");
        if (!mpu.begin())
        {
            ESP_LOGI("app_main", "Failed to boot MPU6050");
            while (1)
                ;
        }
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        ESP_LOGI("app_main", "Initializing MPU6050...done");

        // initialize button
        pinMode(BUTTON_LED, OUTPUT);
        pinMode(BUTTON_PIN, INPUT);

        game3();
        victoryAnimation();
        game1();
        victoryAnimation();
        game2();
        victoryAnimation();
    }
}