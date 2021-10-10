#include <SoftwareSerial.h>

#include "src/StripController.h"

#define LED_TYPE WS2812B
#define DATA_PIN 9
#define COLOR_ORDER GRB
#define NUM_LEDS 153

#define BLT_RX 11
#define BLT_TX 10

CRGB leds[NUM_LEDS];
SoftwareSerial bltSerial(BLT_RX, BLT_TX);
StripController ledController(&bltSerial);

void setup()
{
    bltSerial.begin(38400);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    ledController.setLEDs(leds, NUM_LEDS);
}

void loop()
{
    ledController.mainloop();
}
