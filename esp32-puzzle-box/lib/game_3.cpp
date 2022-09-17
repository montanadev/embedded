#include "Arduino.h"
#include <esp_log.h>
#include <Adafruit_L3GD20_U.h>
#include <vector>
#include "utils.cpp"
#include "led.cpp"

const std::vector<std::vector<int>> *GYRO_RANGES = new std::vector<std::vector<int>>{
    {-250, -150},
    {-150, -50},
    {-50, 50},
    {50, 150},
    {150, 250}};

const std::vector<std::vector<int>> *X_GYRO_PIN_RANGES = new std::vector<std::vector<int>>{
    {-250, -150, 0},
    {-150, -50, 1},
    {-50, 50, 2},
    {50, 150, 3},
    {150, 250, 4}};

const std::vector<std::vector<int>> *Y_GYRO_PIN_RANGES = new std::vector<std::vector<int>>{
    {-250, -150, 5},
    {-150, -50, 6},
    {-50, 50, 7},
    {50, 150, 8},
    {150, 250, 9}};

const std::vector<std::vector<int>> *Z_GYRO_PIN_RANGES = new std::vector<std::vector<int>>{
    {-250, -150, 10},
    {-150, -50, 11},
    {-50, 50, 12},
    {50, 150, 13},
    {150, 250, 14}};

class Game3
{
public:
    explicit Game3(LEDStrip *led, Adafruit_L3GD20_Unified &mpu6050) : led_strip(led), mpu(mpu6050) {}

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

    void displayRangesWithColor(int pos, const std::vector<std::vector<int>> *ranges, uint32_t color)
    {
        for (auto range : *ranges)
        {
            int start = range[0];
            int stop = range[1];
            int pin = range[2];
            if (pos <= stop && pos >= start)
            {
                led_strip->setPixelColor(pin, color);
                break;
            }
        }
        if (pos <= (*ranges)[0][0])
        {
            led_strip->setPixelColor((*ranges)[0][2], color);
        }
        if (pos >= (*ranges)[(*ranges).size() - 1][1])
        {
            led_strip->setPixelColor((*ranges)[(*ranges).size() - 1][2], color);
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

    bool withinRange(int target, int pos, const std::vector<std::vector<int>> *ranges)
    {
        for (auto range : *ranges)
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

    bool checkAndDisplayWithinRange(int target, int pos, const std::vector<std::vector<int>> *ranges)
    {
        if (withinRange(target, pos, GYRO_RANGES))
        {
            // within range, combine into a single green pin
            displayRangesWithColor(pos, ranges, led_strip->blue);
            return true;
        }
        // not within range, draw separate pixels
        displayRangesWithColor(target, ranges, led_strip->red);
        displayRangesWithColor(pos, ranges, led_strip->green);
        return false;
    }

    void run()
    {
        for (int games = 0; games < 3; games++)
        {
            int threshold = 10;

            int xPos = 0;
            int yPos = 0;
            int zPos = 0;

            // find random targets within the -250 < x < -50 and 50 < x < 250 ranges
            // this is because the seeker led always starts at 0, and might trigger
            // an automatic win (no fun)
            int xTarget = randomTarget(50, 250);
            int yTarget = randomTarget(50, 250);
            int zTarget = randomTarget(50, 250);

            led_strip->clear();
            led_strip->setPixelColor(2, led_strip->green);
            led_strip->show();

            while (1)
            {
                sensors_event_t g;
                mpu.getEvent(&g);

                // gyro reads in decimal, convert to big, whole numbers
                int x = floor(g.gyro.x * 100); // x normalizes at -14
                int y = floor(g.gyro.y * 100); // y normalizes at 1
                int z = floor(g.gyro.z * 100); // z at 3

                // ESP_LOGI("game3", "Rotation X: %d, Y: %d, Z: %d", x, y, z);

                xPos += determineMovement(x, -14, threshold) * 1.5;
                yPos += determineMovement(y, 1, threshold) * 1.5;
                zPos += determineMovement(z, 3, threshold) * 1.5;

                // clamp within the -250 < x < 250 range
                xPos = clamp(xPos, -250, 250);
                yPos = clamp(yPos, -250, 250);
                zPos = clamp(zPos, -250, 250);

                // ESP_LOGI("game3", "xTarget=%d xPos=%d", xTarget, xPos);
                // ESP_LOGI("game3", "ypos=%d", yPos);
                // ESP_LOGI("game3", "zpos=%d z=%d", zPos, z);

                led_strip->clear();
                bool xWin = checkAndDisplayWithinRange(xTarget, xPos, X_GYRO_PIN_RANGES);
                bool yWin = checkAndDisplayWithinRange(yTarget, yPos, Y_GYRO_PIN_RANGES);
                bool zWin = checkAndDisplayWithinRange(zTarget, zPos, Z_GYRO_PIN_RANGES);

                if (xWin && yWin && zWin)
                {
                    // if all the leds are within the target ranges, win!
                    break;
                }

                led_strip->show();
                delay(100);
            }
            led_strip->victory();
        }
    }

private:
    LEDStrip *led_strip;
    Adafruit_L3GD20_Unified mpu;
};