#include "src/LEDStripController.h"

#define LED_TYPE WS2812B
#define DATA_PIN 8
#define COLOR_ORDER GRB
#define NUM_LEDS 153

CRGB leds[NUM_LEDS];
LEDStripController::SerialController ledController;

void setup()
{
    Serial.begin(9600);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    ledController.setLEDs(leds, NUM_LEDS);
}

void loop()
{
    ledController.mainloop();
}
