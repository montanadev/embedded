/*
ESP8266 Garage door controller & REST API w/ 2 relay module
*/

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "../lib/secrets.h"

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

int inputPin = 14;
bool running = false;

void handleRoot();

void handleNotFound();

void setup() {
    Serial.begin(115200);

    pinMode(inputPin, OUTPUT);

    // WiFi
    Serial.println("Starting wifi...");
    wifiMulti.addAP(SSID, PASSWORD);
    Serial.println("Starting wifi...done");

    while (wifiMulti.run() != WL_CONNECTED) {
        delay(1000);
        Serial.print('.');
    }
    Serial.println("Connected to\t");
    Serial.println(WiFi.localIP());  // Send the IP address of the ESP8266 to the computer
    Serial.println("WiFi connected");

    server.on("/", HTTP_POST, handleRoot);
    server.onNotFound(handleNotFound);
    server.begin();
}

void loop() {
    if (running) {
        digitalWrite(inputPin, HIGH);
    } else {
        digitalWrite(inputPin, LOW);
    }
    server.handleClient();
}

void handleRoot() {
    if (server.hasArg("start")) {
        running = true;
        server.send(200, "text/plain", "200: command completed");
        return;
    }
    if (server.hasArg("stop")) {
        running = false;
        server.send(200, "text/plain", "200: command completed");
        return;
    }
    running = false;
    server.send(400, "text/plain", "400: Invalid Request");
    return;
}

void handleNotFound() {
    server.send(404, "text/plain",
                "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}