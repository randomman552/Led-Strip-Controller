#ifndef Effects_h
#define Effects_h

#include <Arduino.h>
#include <FastLED.h>

/**
 * Namespace containing all the Effects we can apply to the LED Strip.
 * Before use, the following namespace elements must have a value set:
 * - leds - The leds to apply the effects to.
 * - numLEDs - The number of leds in the strip.
 * - color - The custom color to use for the leds (defaults to white).
 */
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

    /**
    * Custom colour to use when using custom colour functions.
    * Must be set somewhere else in the program.
    */
    extern CRGB color;
    
    void clear();

    /**
     * Collection of functions which use the color Effects::color on the leds:
     * fill - Fill the led strip
     * fade - Fade the led strip in and out
     * fillEmpty - Fill and then empty the led strip
     * fillEmptyMiddle - Same as previous, but from the middle
     * bounce - Group of leds bounce from one end to the other
     */
    namespace CustomColor
    {
        /**
         * Fill the led strip with the current custom color
         */
        void fill();

        /**
         * Fade the current custom color in and out.
         * Function uses 2 bytes of static variables.
         */
        void fade();

        /**
         * Fill the led strip from one end to the other, then empty it in that direction.
         * The function then repeats this in the reverse direction.
         * Function uses 2 bytes of static variables.
         */
        void fillEmpty();

        /**
         * Functions in the same way as fillEmpty, but starts from the center of the led strip.
         */
        void fillEmptyMiddle();

        /**
         * Group of leds of custom colour bounces from one end of the strip to the other.
         */
        void bounce();
    } // namespace CustomColor

    /**
     * Namespace containing a series of rainbow based lighting effects.
     * fill - Fill the led strip with a rainbow gradient
     * fillEmpty - Fill the led strip with a rainbow gradient, then empty it again
     * cycle - Cycle all leds hue at once
     * spinCycle - Cycle each leds hue individually (looks like spinning effect)
     */
    namespace Rainbow
    {
        /**
         * Fill the led strip with a rainbow gradient.
         */
        void fill();

        /**
         * Fill and empty the led strip with a rainbow gradient.
         */
        void fillEmpty();

        /**
         * Cycle ALL leds in the strip with rainbow effect.
         */
        void cycle();

        /**
         * Cycle each led in the strip separately from a starting rainbow gradient.
         * Leads to a spinning effect on the leds.
         */
        void spinCycle();
    } // namespace Rainbow
    
    /**
     * Collection of lighting functions using various randomised elements:
     */
    namespace Random
    {
        //TODO Random lighting functions
    } // namespace Random
}; // namespace effects


#endif