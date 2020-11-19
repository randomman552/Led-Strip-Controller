#include "src/LEDController.h"


LEDController leds;

void setup() {
    Serial.begin(9600);
    Serial.println("Hello there!");
    FastLED.setBrightness(64);
    Serial.println("Setup Complete!");

    CRGB color(128, 128, 128);
    Effects::color = color;
}

void loop() {
    leds.iterate();
    delay(20);
}
