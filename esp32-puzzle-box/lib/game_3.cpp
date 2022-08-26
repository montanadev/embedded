#include "Arduino.h"
#include <esp_log.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MPU6050.h>
#include <vector>
#include "utils.cpp"

std::vector<std::vector<int>> GYRO_RANGES = {
    {-250, -150},
    {-150, -50},
    {-50, 50},
    {50, 150},
    {150, 250}};

std::vector<std::vector<int>> X_GYRO_PIN_RANGES = {
    {-250, -150, 0},
    {-150, -50, 1},
    {-50, 50, 2},
    {50, 150, 3},
    {150, 250, 4}};

std::vector<std::vector<int>> Y_GYRO_PIN_RANGES = {
    {-250, -150, 5},
    {-150, -50, 6},
    {-50, 50, 7},
    {50, 150, 8},
    {150, 250, 9}};

std::vector<std::vector<int>> Z_GYRO_PIN_RANGES = {
    {-250, -150, 10},
    {-150, -50, 11},
    {-50, 50, 12},
    {50, 150, 13},
    {150, 250, 14}};

class Game3
{
public:
    Game3(Adafruit_NeoPixel &led, Adafruit_MPU6050 &mpu6050)
    {
        led_strip = led;
        mpu = mpu6050;
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

    void displayRangesWithColor(int pos, std::vector<std::vector<int>> ranges, uint32_t color)
    {
        for (auto range : ranges)
        {
            int start = range[0];
            int stop = range[1];
            int pin = range[2];
            if (pos <= stop && pos >= start)
            {
                led_strip.setPixelColor(pin, color);
                break;
            }
        }
        if (pos <= ranges[0][0])
        {
            led_strip.setPixelColor(ranges[0][2], color);
        }
        if (pos >= ranges[ranges.size() - 1][1])
        {
            led_strip.setPixelColor(ranges[ranges.size() - 1][2], color);
        }
    }

    int clamp(int i, int low, int high)
    {
        if (i < low)
        {
            return low;
        }
        if (i > high)
        {
            return high;
        }
        return i;
    }

    int randomTarget(int low, int high)
    {
        if (random(0, 1))
        {
            return random(low, high);
        }
        return -1 * random(low, high);
    }

    bool withinRange(int target, int pos, std::vector<std::vector<int>> ranges)
    {
        for (auto range : ranges)
        {
            int start = range[0];
            int stop = range[1];
            // if both the target and pos lie within the same range,
            // then they're sharing the same led pixel space, and are within range
            if (target >= start && target <= stop &&
                pos >= start && pos <= stop)
            {
                return true;
            }
        }
        return false;
    }

    void run()
    {
        uint32_t white = led_strip.Color(0, 0, 0);
        uint32_t red = led_strip.Color(255, 0, 0);
        uint32_t green = led_strip.Color(0, 255, 0);
        uint32_t blue = led_strip.Color(0, 0, 255);

        int threshold = 400;
        int xPos = 0;
        int yPos = 0;
        int zPos = 0;

        int xTarget = randomTarget(50, 250);
        int yTarget = randomTarget(50, 250);
        int zTarget = randomTarget(50, 250);

        clearDisplay(led_strip);
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
            xPos += determineMovement(x, -14, threshold);
            yPos += determineMovement(y, 1, threshold);
            zPos += determineMovement(z, 3, threshold);

            xPos = clamp(xPos, -250, 250);
            yPos = clamp(yPos, -250, 250);
            zPos = clamp(zPos, -250, 250);

            ESP_LOGI("game3", "xTarget=%d xPos=%d", xTarget, xPos);
            // ESP_LOGI("game3", "ypos=%d", yPos);
            // ESP_LOGI("game3", "zpos=%d z=%d", zPos, z);

            clearDisplay(led_strip);

            // X axis
            if (withinRange(xTarget, xPos, GYRO_RANGES))
            {
                // within range, combine into a single green pin
                displayRangesWithColor(xPos, X_GYRO_PIN_RANGES, blue);
            }
            else
            {
                // not within range, draw separate pixels
                displayRangesWithColor(xTarget, X_GYRO_PIN_RANGES, red);
                displayRangesWithColor(xPos, X_GYRO_PIN_RANGES, green);
            }

            // Y axis
            if (withinRange(yTarget, yPos, GYRO_RANGES))
            {
                // within range, combine into a single green pin
                displayRangesWithColor(yPos, Y_GYRO_PIN_RANGES, blue);
            }
            else
            {
                // not within range, draw separate pixels
                displayRangesWithColor(yTarget, Y_GYRO_PIN_RANGES, red);
                displayRangesWithColor(yPos, Y_GYRO_PIN_RANGES, green);
            }

            // Z axis
            if (withinRange(zTarget, zPos, GYRO_RANGES))
            {
                // within range, combine into a single green pin
                displayRangesWithColor(zPos, Z_GYRO_PIN_RANGES, blue);
            }
            else
            {
                // not within range, draw separate pixels
                displayRangesWithColor(zTarget, Z_GYRO_PIN_RANGES, red);
                displayRangesWithColor(zPos, Z_GYRO_PIN_RANGES, green);
            }

            led_strip.show();
            delay(100);
        }
    }

private:
    Adafruit_NeoPixel led_strip;
    Adafruit_MPU6050 mpu;
};