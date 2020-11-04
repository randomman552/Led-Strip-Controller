#ifndef Effects_h
#define Effects_h

#include <Arduino.h>
#include <FastLED.h>


namespace Effects
{
    /**
     * The LED's to apply the Effects to.
     * Must be set before this namespace can be used.
     */
    extern CRGB *leds;
    
    /**
     * Number of LEDs
     * Must be set before Effects namespace functions work
     */
    extern int numLEDs;

    void clear();
}; // namespace effects


#endif