#include "LEDController.h"

#pragma region Constructors

LEDController::LEDController()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(_leds, NUM_LEDS);
    Effects::leds = _leds;
    Effects::numLEDs = NUM_LEDS;
}

#pragma endregion


#pragma region LED Functions

void LEDController::iterate()
{
    Effects::clear();
    FastLED.show();
}

#pragma endregion

