#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <HTTPClient.h>
#include "secrets.h"

#define PIN        23
#define NUMPIXELS 12
#define DELAYVAL 500
#define HOME_URL "http://192.168.1.25:8000/"


Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Sleeping...done");

    Serial.println("Start pixels...");
    pixels.begin();
    Serial.println("Start pixels...done");

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.println("...");
        delay(1000);
    }
    Serial.println("Connected:");
    Serial.println(WiFi.localIP());
    Serial.println("Connecting to WiFi...done");

}

void loop() {
    HTTPClient http;
    http.begin(HOME_URL);

    int response = http.GET();
    if (response > 0) {
        Serial.println("Home responded");
        Serial.println(response);
    } else {
        Serial.println("Got error");
        Serial.println(response);
    }

    Serial.println("Loop pixels...");
    pixels.clear();

    for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
        pixels.show();
        delay(DELAYVAL);
    }
    Serial.println("Loop pixels...done");
}

