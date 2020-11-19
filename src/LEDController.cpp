#include "LEDController.h"

#pragma region Constructors

LEDController::LEDController()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(_leds, NUM_LEDS);
    Effects::leds = _leds;
    Effects::numLEDs = NUM_LEDS;
    Effects::clear();
}

#pragma endregion


#pragma region LED Functions

void LEDController::iterate()
{
    Effects::CustomColor::fillEmpty();
    FastLED.show();
}

#pragma endregion

