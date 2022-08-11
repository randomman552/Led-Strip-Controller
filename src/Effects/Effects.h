#ifndef LEDCON_Effects_h
#define LEDCON_Effects_h

#include <FastLED.h>

#include "../LEDStripController.h"


namespace LEDStripController {
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

        #pragma region Base functions
        void clear(Controller &C);

        /**
         * @brief Basic function to fill the LED strip with a given color
         * 
         * @param C The Controller instance
         * @param col Color to fill with
         */
        void fill(Controller &C, CRGB col);

        void fill (Controller &C, CHSV col);

        /**
         * Base fade function used by other fade functions
         * Used in Color::fade and Random::fade
         */
        void fade(Controller &C, CRGB col);

        void fade(Controller &C, CHSV col);

        /**
         * Base fillEmpty function used by other fillEmpty functions
         * Used in Color::fillEmpty and Random::fillEmpty
         */
        void fillEmpty(Controller &C, CRGB col);
       
        void fillEmpty(Controller &C, CHSV col);

        /**
         * Base fillEmptyMiddle function used by other fillEmptyMiddle functions
         * Used in Color::fillEmptyMiddle and Random::fillEmptyMiddle
         */
        void fillEmptyMiddle(Controller &C, CRGB col);

        void fillEmptyMiddle(Controller &C, CHSV col);

        #pragma endregion

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
             * Fill strip with alternating colours
             * Starts at curCol
             * Ends at finCol
             * Repeats sequence until strip is filled
             */
            void alternateFill(Controller &C);

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

        /**
         * @brief Lighting functions using random colors
         */
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
            void fill(Controller &C);

            /**
             * Fade in random color, then fade it out
             */
            void fade(Controller &C);

            /**
             * Fill with random color, then empty
             */
            void fillEmpty(Controller &C);

            /**
             * Fill with random color from edges, then empty to middle
             */
            void fillEmptyMiddle(Controller &C);
        } // namespace Random
        
    }; // namespace Effects
};

#endif
