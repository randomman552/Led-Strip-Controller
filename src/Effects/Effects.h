#ifndef LEDCON_Effects_h
#define LEDCON_Effects_h

#include <FastLED.h>

#include "../StripController.h"

/**
 * Clamp helper function.
 * Clamps given integer to be within the given range (inclusive)
 * i - Variable to clamp
 * max - Maximum value
 * min - Minimum value
 */
int clamp(int val, int min, int max);


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

    void clear(StripController &C);

    /**
     * Base fade function used by other fade functions
     * Used in Color::fade and Random::fade
     */
    void fade(StripController &C, CRGB col);
    void fade(StripController &C, CHSV col);

    /**
     * Base fillEmpty function used by other fillEmpty functions
     * Used in Color::fillEmpty and Random::fillEmpty
     */
    void fillEmpty(StripController &C, CRGB col);
    void fillEmpty(StripController &C, CHSV col);

    /**
     * Base fillEmptyMiddle function used by other fillEmptyMiddle functions
     * Used in Color::fillEmptyMiddle and Random::fillEmptyMiddle
     */
    void fillEmptyMiddle(StripController &C, CRGB col);
    void fillEmptyMiddle(StripController &C, CHSV col);

    /**
     * Lighting functions using custom color
     */
    namespace Color
    {   
        /**
         * Fill the LED with the current set colour.
         */
        void fill(StripController &C);
        /**
         * Fill strip with alternating colours
         * Starts at curCol
         * Ends at finCol
         * Repeats sequence until strip is filled
         */
        void alternateFill(StripController &C);
        /**
         * Fade the given color in and out
         */
        void fade(StripController &C);
        /**
         * Fill and empty the LED strip from end to end
         */
        void fillEmpty(StripController &C);
        /**
         * Fill and empty the LED strip from the end's to the middle
         */
        void fillEmptyMiddle(StripController &C);
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
        void fill(StripController &C);
        /**
         * Fill and empty the LED strip with a rainbow gradient from end to end
         */
        void fillEmpty(StripController &C);
        /**
         * Fill the LED strip with a hue.
         * Advance hue by 1 each frame, showly shifting through all colors
         */
        void cycle(StripController &C);
        /**
         * Fill LED strip with rainbow gradient
         * Rotate rainbow gradient by 1 each frame
         */
        void spinCycle(StripController &C);
    } // namespace Rainbow

    namespace Random
    {
        static CHSV color = CHSV(random8(), 255, 255);
        /**
         * Randomise current Random color
         */
        void randomise();
        /**
         * Fill led strip with random color
         */
        void fill(StripController &C);
        /**
         * Fade in random color, then fade it out
         */
        void fade(StripController &C);
        /**
         * Fill with random color, then empty
         */
        void fillEmpty(StripController &C);
        /**
         * Fill with random color from edges, then empty to middle
         */
        void fillEmptyMiddle(StripController &C);
    } // namespace Random
    
}; // namespace Effects


#endif
