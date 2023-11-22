#include <Adafruit_SSD1306.h>

static const uint8_t minidatadog[] = {0x00, 0x03, 0x00, 0x18, 0x46, 0x80, 0x7d, 0xff, 0xc0, 0x7f, 0xff, 0x60, 0xfb,
                                      0xff, 0x00, 0xfb, 0xfe, 0x80, 0x7b, 0x9e, 0x00, 0x07, 0x9f, 0x80, 0x1f, 0xff,
                                      0xc0, 0x1f, 0xff, 0x60, 0x1f, 0xfe, 0x60, 0x0f, 0xff, 0xe0, 0x07, 0xff, 0xce,
                                      0x03, 0xe1, 0xfe, 0x03, 0xbf, 0xfc, 0x07, 0xbf, 0xf9, 0x0f, 0xbf, 0x89, 0x1b,
                                      0xff, 0x81, 0x3e, 0xf8, 0x01, 0x7f, 0x70, 0x01, 0x7f, 0x10, 0x3e, 0x1f, 0x1f,
                                      0x80, 0x1f, 0x00, 0x00, 0x0f, 0x00, 0x00};


void showSettings(Adafruit_SSD1306 display, int ic, int selection) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.drawBitmap(104, 0, minidatadog, 24, 24, 1);
    display.setTextSize(2);
    display.setFont(NULL);
    display.setCursor(0, 0);
    display.println("Settings");
    display.drawLine(0, 15, 104, 15, 1);
    display.setTextSize(1);

    display.setCursor(0, 17);
    if (selection == 0) {
        display.setTextColor(BLACK, WHITE);
    } else {
        display.setTextColor(WHITE);
    }
    display.println("Set clock");

    display.setCursor(0, 27);
    if (selection == 1) {
        display.setTextColor(BLACK, WHITE);
    } else {
        display.setTextColor(WHITE);
    }
    display.println("Enable WiFi");
    display.display();
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

