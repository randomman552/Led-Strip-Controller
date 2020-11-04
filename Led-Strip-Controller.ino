#include "src/LEDController.h"


LEDController leds;

void setup() {
    Serial.begin(9600);
    Serial.println("Hello there!");
    FastLED.setBrightness(128);
    Serial.println("Setup Complete!");
}

void loop() {
    leds.iterate();
    delay(100);
}
