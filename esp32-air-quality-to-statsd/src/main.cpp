#include "Arduino.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <HardwareSerial.h>
#include "Adafruit_PM25AQI.h"
#include <esp_log.h>
#include <esp_wifi.h>
#include "../lib/secrets.h"

extern "C"
{
    HardwareSerial SerialPort(2);

    Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

    void connect_wifi()
    {
        while (1)
        {
            int attempts = 0;
            WiFi.mode(WIFI_STA);
            WiFi.begin(SSID, PASSWORD);
            ESP_LOGI("connect_wifi", "Connecting to wifi...\n");
            while (WiFi.status() != WL_CONNECTED && attempts < 7)
            {
                attempts++;
                ESP_LOGI("connect_wifi", ".\n");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            if (WiFi.status() != WL_CONNECTED)
            {
                continue;
            }
            break;
        }
        ESP_LOGI("connect_wifi", "Connected @ %s\n", WiFi.localIP().toString().c_str());
    }

    void send_udp_packet(String host, int port, String key, int value)
    {
        char s[128];
        sprintf(s, "particulate.%s:%d|g|#%s", key.c_str(), value, TAG);
        String message = s;

        WiFiUDP udp;
        udp.beginPacket(UDP_HOST, UDP_PORT);
        udp.write((uint8_t *)message.c_str(), message.length());
        udp.endPacket();
    }

    void app_main(void)
    {
        initArduino();
        connect_wifi();

        SerialPort.begin(9600, SERIAL_8N1, 16, 17);

        ESP_LOGI("app_main", "Connecting to PM25AQI...");
        if (!aqi.begin_UART(&SerialPort))
        {
            ESP_LOGW("app_main", "Failed to connect to PM25AQI");
        }
        ESP_LOGI("app_main", "Connecting to PM25AQI...done");

        PM25_AQI_Data data;
        while (1)
        {
            if (!aqi.read(&data))
            {
                ESP_LOGW("app_main", "Failed to read PM25AQI");
                delay(3500);
                continue;
            }

            ESP_LOGI("app_main", "%d", data.pm25_standard);

            send_udp_packet(UDP_HOST, UDP_PORT, "pm25", data.pm25_standard);
            send_udp_packet(UDP_HOST, UDP_PORT, "pm1", data.pm10_standard);
            send_udp_packet(UDP_HOST, UDP_PORT, "pm10", data.pm100_standard);
            delay(3000);
        }
    }
}