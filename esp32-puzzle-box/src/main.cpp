#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <esp_log.h>
#include <vector>
#include "../lib/game_1.cpp"
#include "../lib/game_2.cpp"
#include "../lib/game_3.cpp"
#include "../lib/game_4.cpp"

extern "C"
{
#define NEOPIXEL_LED_PIN 23
#define NEOPIXEL_LED_COUNT 20

    // Adafruit 7seg
    Adafruit_7segment matrix = Adafruit_7segment();
    // Adafruit_MPU6050 xyz gyro
    Adafruit_L3GD20_Unified mpu = Adafruit_L3GD20_Unified(20);
    // Adafruit_VL53L0X depth sensor
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();
    // Adafruit_NeoPixel
    Adafruit_NeoPixel led_strip(NEOPIXEL_LED_COUNT, NEOPIXEL_LED_PIN, NEO_GRB + NEO_KHZ800);
    uint32_t white = led_strip.Color(0, 0, 0);
    uint32_t red = led_strip.Color(255, 0, 0);
    uint32_t green = led_strip.Color(0, 255, 0);
    uint32_t blue = led_strip.Color(0, 0, 255);

    void game1()
    {
        Game1 g = Game1(led_strip);
        g.run();
    }

    void game2()
    {
        Game2 g = Game2(led_strip, lox);
        g.run();
    }

    void game3()
    {
        Game3 g = Game3(led_strip, mpu);
        g.run();
    }

    void app_main()
    {
        initArduino();

        esp_log_level_set("gpio", ESP_LOG_NONE);

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
        }
        ESP_LOGI("app_main", "Initializing VL53L0X...done");

        // initialize gyro
        ESP_LOGI("app_main", "Initializing MPU6050...");
        if (!mpu.begin())
        {
            ESP_LOGI("app_main", "Failed to boot MPU6050");
            while (1)
                ;
        }
        ESP_LOGI("app_main", "Initializing MPU6050...done");

        // initialize button
        pinMode(BUTTON_LED, OUTPUT);
        pinMode(BUTTON_PIN, INPUT);

        // initialize motor
        pinMode(MOTOR_PIN, OUTPUT);

        // initalize sound
        // pinMode(SOUND_PIN, INPUT);

        // initialize 7seg
        matrix.begin(0x70);

        for (uint16_t counter = 0; counter < 9999; counter++)
        {
            matrix.println(counter);
            matrix.writeDisplay();
            delay(10);
        }

        matrix.writeDisplay();

        // initialize games
        Game1 g1 = Game1(led_strip);
        Game2 g2 = Game2(led_strip, lox);
        Game3 g3 = Game3(led_strip, mpu);
        Game4 g4 = Game4();

        // play!

        g1.run();
        g2.run();
        g3.run();
        g4.run();
    }
}