#include "time.h"
#include <Adafruit_SSD1306.h>

void setClock(Adafruit_SSD1306 *display, int ic, int selection, int hour_override, int min_override) {
    display->clearDisplay();
    display->setTextColor(WHITE);
    display->setTextSize(4);
    display->setFont(NULL);
    display->setCursor(5, 10);

    struct tm timeinfo;
    time_t now;
    time(&now);
    setenv("TZ", "UTC-7", 1);
    tzset();

    localtime_r(&now, &timeinfo);

    String hour = "%d";
    if (hour_override < 10) {
        hour = "0%d";
    }
    String min = "%d";
    if (min_override < 10) {
        min = "0%d";
    }

    char code_without_blink[40];
    sprintf(code_without_blink, "%s:%s", hour.c_str(), min.c_str());
    char code_with_blink[40];
    sprintf(code_with_blink, "%s %s", hour.c_str(), min.c_str());

    if (ic % 30 == 0) {
        display->printf(code_with_blink, hour_override, min_override);
    } else {
        display->printf(code_without_blink, hour_override, min_override);
    }

    switch (selection) {
        case 0:
            display->drawLine(5, 50, 25, 50, 1);
            break;
        default:
            display->drawLine(105, 50, 120, 50, 1);
    }

    display->display();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void showClock(Adafruit_SSD1306 *display, int ic) {
    display->clearDisplay();
    display->setTextColor(WHITE);
    display->setTextSize(4);
    display->setFont(NULL);
    display->setCursor(5, 10);

    struct tm timeinfo;
    time_t now;
    time(&now);
    setenv("TZ", "UTC-7", 1);
    tzset();

    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_hour > 12) {
        timeinfo.tm_hour -= 12;
    }

    String hour = "%d";
    if (timeinfo.tm_hour < 10) {
        hour = "0%d";
    }
    String min = "%d";
    if (timeinfo.tm_min < 10) {
        min = "0%d";
    }

    char code_without_blink[40];
    sprintf(code_without_blink, "%s:%s", hour.c_str(), min.c_str());
    char code_with_blink[40];
    sprintf(code_with_blink, "%s %s", hour.c_str(), min.c_str());

    if (ic % 2 == 0) {
        display->printf(code_with_blink, timeinfo.tm_hour, timeinfo.tm_min);
    } else {
        display->printf(code_without_blink, timeinfo.tm_hour, timeinfo.tm_min);
    }

    display->display();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
