/*
ESP8266 Garage door controller & REST API w/ 2 relay module
*/

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "../lib/secrets.h"

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

#define LED 2 // define blinking LED pin
const int inputPinA = 5; // GPIO5 pin of NodeMCU esp8266
const int inputPinB = 4; // GPIO4 pin of NodeMCU esp8266

void handleRoot();

void handleNotFound();

void setup() {
    Serial.begin(115200);

    pinMode(LED, OUTPUT); // Initialize the LED pin as an output
    pinMode(inputPinA, OUTPUT);
    pinMode(inputPinB, OUTPUT);

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
    digitalWrite(inputPinA, HIGH);
    digitalWrite(inputPinB, HIGH);
    server.handleClient();
}

void handleRoot() {
    if (server.hasArg("toggle")) {
        if (server.hasArg("door1")) {
            digitalWrite(inputPinA, LOW);
            delay(server.arg("time").toInt());
            digitalWrite(inputPinA, HIGH);
        }
        if (server.hasArg("door2")) {
            digitalWrite(inputPinB, LOW);
            delay(server.arg("time").toInt());
            digitalWrite(inputPinB, HIGH);
        }
        digitalWrite(inputPinA, HIGH);
        digitalWrite(inputPinB, HIGH);
        server.send(200, "text/plain", "200: command completed");
        return;
    }
    digitalWrite(inputPinA, HIGH);
    digitalWrite(inputPinB, HIGH);
    server.send(400, "text/plain", "400: Invalid Request");
    return;
}

void handleNotFound() {
    server.send(404, "text/plain",
                "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}