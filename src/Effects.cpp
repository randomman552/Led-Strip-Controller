#include "Effects.h"

int Effects::numLEDs = 0;
CRGB *Effects::leds;

void Effects::clear()
{
    CRGB color(0, 0, 0);
    fill_solid(leds, numLEDs, color);
}
