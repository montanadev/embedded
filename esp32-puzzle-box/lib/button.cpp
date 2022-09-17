#pragma once

#include "Arduino.h"

#define BUTTON_PIN 4

bool readButton()
{
    int buttonState = digitalRead(BUTTON_PIN);
    return buttonState == HIGH;
}