#ifndef LEDController_h
#define LEDController_h

#include <Arduino.h>
#include <FastLED.h>
#include <ArxContainer.h>
#include "Effects.h"


// Define LED Setup Constants
#define DATA_PIN 9
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 144


class LEDController
{
private:
    /**
    * FastLED leds to control.
    */
    CRGB _leds[NUM_LEDS];

public:
    LEDController();

    /**
     * Iterate the LED strip state, according to whatever lighting effect is active.
     */
    void iterate();
};

#endif