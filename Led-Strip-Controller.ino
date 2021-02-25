#include "src/LedStripController/Controller.h"

#define LED_TYPE WS2812B
#define DATA_PIN 9
#define COLOR_ORDER GRB
#define NUM_LEDS 153

#define BLT_RX 11
#define BLT_TX 10

CRGB leds[NUM_LEDS];
Controller ledController(BLT_RX, BLT_TX);

void setup()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    ledController.setLEDs(leds, NUM_LEDS);
}

void loop()
{
    ledController.mainloop();
}
