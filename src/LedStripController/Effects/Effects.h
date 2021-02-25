#ifndef LEDCON_Effects_h
#define LEDCON_Effects_h

#include <FastLED.h>

#include "../Controller.h"


/**
 * Namespace containing lighting functions
 * ALL functions match the following function signature: void func(Controller &C)
 */
namespace Effects
{
    // Iterator value for lighting functions
    static int i = 0;
    // Boolean to control iteration direction for lighting functions
    static bool reverse = false;

    void clear(Controller &C);
    /**
     * Clamp the given variable to be within the given range
     * i - Variable to clamp
     * max - Maximum value
     * min - Minimum value
     */
    int clamp(int val, int max, int min);

    /**
     * Lighting functions using custom color
     */
    namespace Color
    {   
        /**
         * Fill the LED with the current set colour.
         */
        void fill(Controller &C);
        /**
         * Fade the given color in and out
         */
        void fade(Controller &C);
        /**
         * Fill and empty the LED strip from end to end
         */
        void fillEmpty(Controller &C);
        /**
         * Fill and empty the LED strip from the end's to the middle
         */
        void fillEmptyMiddle(Controller &C);
    } // namespace Color

    /**
     * Lighting functions displaying rainbow variations
     */
    namespace Rainbow
    {
        static int startHue = 0;

        /**
         * Fill the LED strip with a rainbow gradient
         */
        void fill(Controller &C);
        /**
         * Fill and empty the LED strip with a rainbow gradient from end to end
         */
        void fillEmpty(Controller &C);
        /**
         * Fill the LED strip with a hue.
         * Advance hue by 1 each frame, showly shifting through all colors
         */
        void cycle(Controller &C);
        /**
         * Fill LED strip with rainbow gradient
         * Rotate rainbow gradient by 1 each frame
         */
        void spinCycle(Controller &C);
    } // namespace Rainbow
}; // namespace Effects


#endif
