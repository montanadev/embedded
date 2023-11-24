#include <Adafruit_SSD1306.h>

void displayTime(Adafruit_SSD1306 *display, int ic, int hour, int min) {
    display->clearDisplay();
    display->setTextColor(WHITE);
    display->setTextSize(4);
    display->setFont(NULL);
    display->setCursor(5, 15);

    if (hour > 12) {
        hour = 1;
    }
    if (hour == 0) {
        hour = 12;
    }

    String hour_fmt = "%d";
    if (hour < 10) {
        hour_fmt = "0%d";
    }
    String min_fmt = "%d";
    if (min < 10) {
        min_fmt = "0%d";
    }

    char code_without_blink[40];
    sprintf(code_without_blink, "%s:%s", hour_fmt.c_str(), min_fmt.c_str());
    char code_with_blink[40];
    sprintf(code_with_blink, "%s %s", hour_fmt.c_str(), min_fmt.c_str());

    // it just... feels right
    if (ic % 40 < 20) {
        display->printf(code_with_blink, hour, min);
    } else {
        display->printf(code_without_blink, hour, min);
    }
}

void setClock(Adafruit_SSD1306 *display, int ic, int selection, int hour, int min) {
    displayTime(display, ic, hour, min);

    if (selection == 0) {
        display->drawLine(5, 50, 25, 50, 1);
    } else {
        display->drawLine(105, 50, 120, 50, 1);
    }

    display->display();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

void showClock(Adafruit_SSD1306 *display, int ic, int hour, int min) {
    displayTime(display, ic, hour, min);

    display->display();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
