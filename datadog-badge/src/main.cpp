#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <esp_log.h>
#include "nvs_flash.h"
#include "nvs.h"
#include <Arduino.h>
#include <TimeLib.h>
#include "../lib/clock.cpp"
#include "../lib/settings.cpp"
#include "../lib/wifi.cpp"
#include <WiFi.h>
#include <ESPNtpClient.h>


// BUTTON_PIN is the pin on the Datadog paw
#define BUTTON_PIN                    26
// SCREEN_ADDRESS is normally 0x3D for 128x64, but for these crazy Amazon OLEDs, its 0x3C
#define SCREEN_ADDRESS                0x3C
// SETTINGS_OPTIONS
#define SETTINGS_OPTIONS              4
// BUTTON_LONGPRESS_THRESHOLD_MS
#define BUTTON_LONGPRESS_THRESHOLD_MS 250

static const uint8_t datadog[] = {0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x40,
                                  0x1f, 0xbc, 0x00, 0x00, 0x07, 0xfc, 0x00, 0xb8, 0x3f, 0xbe, 0x00, 0x00, 0x1f, 0xfe,
                                  0x03, 0xff, 0xff, 0xbf, 0x00, 0x00, 0x3f, 0xf7, 0x1f, 0xff, 0xff, 0x9f, 0x00, 0x00,
                                  0x3f, 0xff, 0xff, 0xff, 0xff, 0xcf, 0x80, 0x00, 0x7f, 0xfb, 0xff, 0xff, 0xfe, 0xcf,
                                  0x80, 0x00, 0x7f, 0xfb, 0xff, 0xff, 0xff, 0x47, 0xc0, 0x00, 0x7f, 0xff, 0xff, 0xff,
                                  0xff, 0x81, 0x80, 0x00, 0xff, 0xfd, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xfd,
                                  0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x7f, 0xfd, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00,
                                  0x7f, 0xfc, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x3f, 0xfc, 0xff, 0xff, 0xff, 0x08,
                                  0x00, 0x00, 0x1f, 0xfc, 0xff, 0x87, 0xff, 0x08, 0x00, 0x00, 0x0f, 0xf8, 0xff, 0x07,
                                  0xff, 0x88, 0x00, 0x00, 0x07, 0xf0, 0xfe, 0x03, 0xff, 0xf8, 0x00, 0x00, 0x00, 0xc1,
                                  0xfe, 0x03, 0xff, 0xf8, 0x00, 0x00, 0x01, 0x03, 0xfe, 0x07, 0xff, 0xfc, 0x00, 0x00,
                                  0x01, 0xff, 0xff, 0x3f, 0xff, 0xfe, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff,
                                  0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x01, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0x80, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x81, 0xc0, 0x00, 0x00, 0xff,
                                  0xff, 0xff, 0xfe, 0x01, 0xc0, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfe, 0x03, 0xc0, 0x00,
                                  0x00, 0xff, 0xff, 0xff, 0xff, 0x03, 0xc0, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc7,
                                  0xc0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xfe, 0xff,
                                  0xff, 0xef, 0x80, 0x00, 0x00, 0x0f, 0xff, 0x3f, 0xff, 0xdf, 0x80, 0x1c, 0x00, 0x03,
                                  0xff, 0xcf, 0xff, 0x8c, 0x07, 0xfc, 0x00, 0x03, 0xff, 0xe3, 0xff, 0x01, 0xff, 0xfc,
                                  0x00, 0x01, 0xff, 0xf8, 0x78, 0x7f, 0xff, 0xfc, 0x00, 0x01, 0xff, 0x00, 0x00, 0xff,
                                  0xff, 0xfc, 0x00, 0x01, 0xff, 0x1f, 0x83, 0xff, 0xff, 0xec, 0x00, 0x01, 0xff, 0x3f,
                                  0xff, 0xff, 0xff, 0xee, 0x00, 0x01, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xce, 0x00, 0x03,
                                  0xff, 0xbf, 0xff, 0xff, 0xff, 0x8e, 0x00, 0x03, 0xff, 0xbf, 0xff, 0xff, 0x7f, 0x86,
                                  0x00, 0x07, 0xff, 0x9f, 0xff, 0xff, 0x3f, 0x06, 0x00, 0x0f, 0xff, 0x9f, 0xff, 0xfe,
                                  0x0f, 0x06, 0x00, 0x3f, 0xff, 0x9f, 0xff, 0xfc, 0x02, 0x06, 0x00, 0x7f, 0xff, 0x9f,
                                  0xff, 0xf8, 0x00, 0x06, 0x00, 0xe0, 0xff, 0xdf, 0xff, 0xf8, 0x00, 0x06, 0x03, 0xff,
                                  0x9f, 0xdf, 0xff, 0xf0, 0x00, 0x06, 0x07, 0xff, 0xcf, 0xdf, 0xf1, 0xe0, 0x00, 0x06,
                                  0x07, 0xff, 0xe7, 0xcf, 0xe0, 0x00, 0x00, 0x07, 0x0f, 0xff, 0xf3, 0xcf, 0xc0, 0x00,
                                  0x00, 0x07, 0x0f, 0xff, 0xf9, 0xcf, 0xc0, 0x00, 0x00, 0x03, 0x1f, 0xff, 0xf9, 0xcf,
                                  0x80, 0x00, 0x00, 0x03, 0x1f, 0xff, 0xfc, 0xef, 0x00, 0x00, 0x00, 0x7f, 0x0f, 0xff,
                                  0xfc, 0x0e, 0x00, 0x00, 0x1f, 0xfe, 0x0f, 0xff, 0xfc, 0x0e, 0x00, 0x0f, 0xff, 0x80,
                                  0x03, 0xff, 0xfe, 0x06, 0x03, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xfe, 0x06, 0xff, 0xf0,
                                  0x00, 0x00, 0x00, 0xff, 0xfe, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x07,
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
                                  0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00};


struct State {
    String page;
    int selection;
    int interruptClock;
    unsigned long startPress;
    int hour_override;
    int min_override;
    bool awaitingLow;
};

State s = {"clock", 0, 0, millis(), -1, -1, false};

Adafruit_SSD1306 display(128, 64, &Wire, -1);
ApCredentials *credentials;

long settings_delay;

void IRAM_ATTR button_interrupt() {
    if (digitalRead(BUTTON_PIN) == HIGH) {
        s.startPress = millis();
        s.awaitingLow = true;
        return;
    }

    bool longPress = false;
    long duration = millis() - s.startPress;

    // DEBUGGING
    char stuff[20];
    sprintf(stuff, "%ld", duration);
    ets_printf(stuff);
    ets_printf("\n");
    // DEBUGGING

    if (duration > BUTTON_LONGPRESS_THRESHOLD_MS) {
        longPress = true;
    }

    // can't use ESP_LOGI in an interrupt
    if (longPress) {
        ets_printf("Interrupted with longpress\n");
    } else {
        ets_printf("Interrupted\n");
    }

    if (longPress) {
        if (s.page == "settings") {
            if (s.selection == 0) { // set clock
                ets_printf("settings => set_clock\n");
                s.page = "set_clock";
                s.hour_override = hour();
                s.min_override = minute();
                s.selection = 0;
            } else if (s.selection == 1) { // enable wifi
                ets_printf("settings => wifi\n");
                credentials = getSsidAndPassword();
                s.page = "wifi";
                s.selection = 0;
            } else if (s.selection == 2) { // reset
                ets_printf("settings => reset\n");
                nvs_flash_erase();
                delay(5000);
                esp_restart();
            } else if (s.selection == 3) { // back
                s.page = "clock";
                s.selection = 0;
            }
        } else if (s.page == "set_clock") {
            ets_printf("set_clock => next digit\n");
            s.selection++;
            if (s.selection >= 2) {
                ets_printf("set_clock => clock\n");
                s.page = "clock";
                s.selection = 0;
                setTime(s.hour_override, s.min_override, 0, 1, 1, 2023);
            }
        }
    } else { // shortpress
        if (s.page == "clock") {
            ets_printf("clock => settings\n");
            s.page = "settings";
            s.selection = 0;
            settings_delay = millis();
        } else if (s.page == "settings") {
            ets_printf("settings => incrementing selection\n");
            s.selection = (s.selection + 1) % SETTINGS_OPTIONS;
            settings_delay = millis();
        } else if (s.page == "set_clock") {
            switch (s.selection) {
                case 0:
                    // hour digit
                    s.hour_override++;
                    if (s.hour_override > 12) {
                        s.hour_override = 1;
                    }
                    break;
                default:
                    // min digit
                    s.min_override++;
                    if (s.min_override > 59) {
                        s.min_override = 0;
                    }
            }
        }
    }

    s.interruptClock++;
}

void main_render_loop(void *pvParameter) {
    while (1) {
        //ESP_LOGI("main_render_loop", "(%d) Running main render loop...", s.interruptClock);
        if (s.page == "clock") {
            showClock(&display, s.interruptClock);
        }
        if (s.page == "settings") {
            if (millis() - settings_delay > 5000) {
                s.page = "clock";
            } else {
                showSettings(&display, s.interruptClock, s.selection);
            }
        }
        if (s.page == "set_clock") {
            setClock(&display, s.interruptClock, s.selection, s.hour_override, s.min_override);
        }
        if (s.page == "wifi") {
            showWifi(&display, credentials);
        }
        s.interruptClock++;
        //ESP_LOGI("main_render_loop", "Running main render loop...done");
    }
}

extern "C" void app_main() {
    initArduino();

    ESP_LOGI("app_main", "Initializing nvs memory...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    ESP_LOGI("app_main", "Initializing nvs memory...done");

    ESP_LOGI("app_main", "Initializing screen...");
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        ESP_LOGI("app_main", "SSD1306 allocation failed");
        for (;;); // Don't proceed, loop forever
    }
    ESP_LOGI("app_main", "Initializing screen...done");

    // initialize the push pin
    //pinMode(BUTTON_PIN, INPUT);

    // draw the loading image
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.drawBitmap(30, 0, datadog, 64, 64, 1);
    display.display();
    sleep(2);

    // if there's wifi creds, use them
    wifiStartInStationMode();
    if (isWifiStationMode()) {
        ESP_LOGI("app_main", "Starting NTP client");
        int timezone = get_timezone();
        configTime(3600 * timezone, 0, ntpServer);
        NTP.setInterval (300);
        NTP.begin();
    }

    attachInterrupt(BUTTON_PIN, button_interrupt, CHANGE);

    xTaskCreate(&main_render_loop, "main_render", 10000, NULL, 0, NULL);
}



