#include "time.h"
#include "Adafruit_LEDBackpack.h"

// ntp settings
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void showClock(Adafruit_7segment matrix)
{
    // initalize ntp
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

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
        int displayValue = timeinfo.tm_hour * 100 + timeinfo.tm_min;
        if (timeinfo.tm_hour == 0)
        {
            displayValue += 1200;
        }
        matrix.print(displayValue, DEC);

        for (int i = 0; i < 60; i++)
        {
            blink = !blink;
            matrix.drawColon(blink);
            matrix.writeDisplay();
            delay(1000);
        }
    }
}