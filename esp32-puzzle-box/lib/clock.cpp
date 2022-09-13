#pragma once

#include "time.h"
#include "Adafruit_LEDBackpack.h"
#include "nvs.cpp"

// ntp settings
const char *ntpServer = "pool.ntp.org";

int getTimezone() {
    return nvs_read_int("timezone", -5);
}

void setTimezone(int timezone) {
    nvs_write_int("timezone", timezone);
}

void showClock(Adafruit_7segment matrix, int timezone)
{
    // initalize ntp
    configTime(3600 * timezone, 0, ntpServer);

    bool blink = false;
    while (1)
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            ESP_LOGI("showClock", "Failed to get time");
            delay(1000);
            continue;
        }
        if (timeinfo.tm_hour > 12) {
            timeinfo.tm_hour -= 12;
        }
        int displayValue = timeinfo.tm_hour * 100 + timeinfo.tm_min;
        if (timeinfo.tm_hour == 0)
        {
            displayValue += 1200;
        }
        matrix.print(displayValue, DEC);

        // spend 60s blinking colon before requesting ntp time again
        for (int i = 0; i < 60; i++)
        {
            blink = !blink;
            matrix.drawColon(blink);
            matrix.writeDisplay();
            delay(1000);
        }
    }
}