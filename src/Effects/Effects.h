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

        void clear(Controller &C)
        {
            CRGB col(0, 0, 0);
            fill_solid(C.getLEDs(), C.getNumLEDs(), col);
        };

        /**
         * Base fade function used by other fade functions
         * Used in Color::fade and Random::fade
         */
        void fade(Controller &C, CRGB col)
        {
            // Ensure i is within range
            i = clamp(i, 0, 255);

            col.r = col.r * ((float)i / 255);
            col.g = col.g * ((float)i / 255);
            col.b = col.b * ((float)i / 255);

            fill_solid(C.getLEDs(), C.getNumLEDs(), col);
            
            // Iterate i depending on reverse boolean
            if (reverse) i--; else i++;

            // If i has reached a limit, invert reverse and clamp i
            // Advance to the next color if we have faded out
            if (i == 255) { 
                reverse = !reverse;
            } else if (i == 0) {
                reverse = !reverse;
                C.advanceColor();
            }
        };

        void fade(Controller &C, CHSV col)
        {
            CRGB rgbCol;
            hsv2rgb_rainbow(col, rgbCol);
            fade(C, rgbCol);
        };

        /**
         * Base fillEmpty function used by other fillEmpty functions
         * Used in Color::fillEmpty and Random::fillEmpty
         */
        void fillEmpty(Controller &C, CRGB col)
        {
            i = clamp(i, 0, C.getNumLEDs() * 2);
            clear(C);

            int start;
            int length;
            //Calculate appropriate start and length depending on whether we are past the max led number
            if (i > C.getNumLEDs())
            {
                start = i - C.getNumLEDs();
                length = C.getNumLEDs() - (i - C.getNumLEDs());
            } else {
                start = 0;
                length = i;
            }
            fill_solid(C.getLEDs() + start, length, col);

            if (reverse) i--; else i++;
            if (i == C.getNumLEDs() * 2 || i == 0) {
                reverse = !reverse;
                // Advance color when reaching end of strip
                C.advanceColor();
            }
        };
       
        void fillEmpty(Controller &C, CHSV col)
        {
            CRGB rgbCol;
            hsv2rgb_rainbow(col, rgbCol);
            fillEmpty(C, rgbCol);
        };

        /**
         * Base fillEmptyMiddle function used by other fillEmptyMiddle functions
         * Used in Color::fillEmptyMiddle and Random::fillEmptyMiddle
         */
        void fillEmptyMiddle(Controller &C, CRGB col)
        {
            i = clamp(i, 0, C.getNumLEDs());
            clear(C);

            int mid = C.getNumLEDs() / 2;

            //Fill the led strip appropriately depending on our iteration status
            if (i > mid) {
                //Filling from edges
                fill_solid(C.getLEDs() + i - mid, C.getNumLEDs() - (i - mid) * 2, col);
            } else {
                //Emptying to middle
                fill_solid(C.getLEDs(), i, col);
                fill_solid(C.getLEDs() + (C.getNumLEDs() - i), i, col);
            }
            
            if (reverse) i--; else i++;
            if (i == C.getNumLEDs() || i == 0) { 
                reverse = !reverse;
                C.advanceColor();
            }
        };

        void fillEmptyMiddle(Controller &C, CHSV col)
        {
            CRGB rgbCol;
            hsv2rgb_rainbow(col, rgbCol);
            fillEmptyMiddle(C, rgbCol);
        };

        #pragma endregion

        /**
         * Lighting functions using custom color
         */
        namespace Color
        {   
            /**
             * Fill the LED with the current set colour.
             */
            void fill(Controller &C)
            {
                i = clamp(i, 0, 255);
                fill_solid(C.getLEDs(), C.getNumLEDs(), C.getColor());
                
                // Advance color after set duration (255 frames)
                if (i == 255) {
                    i = 0;
                    C.advanceColor();
                }
                i++;
            };
            
            /**
             * Fill strip with alternating colours
             * Starts at curCol
             * Ends at finCol
             * Repeats sequence until strip is filled
             */
            void alternateFill(Controller &C)
            {
                int start = C.getCurrentColorIndex();
                int numCols = C.getFinalColorIndex() + 1 - start;
                CRGB *leds = C.getLEDs();
                int numLEDs = C.getNumLEDs();
                int offset = C.getColOffset();

                for (int i = 0; i < numLEDs; i++)
                {
                    // Math to work out color alternations
                    // Modulo operators keep valus within numCols range
                    // Offset gives the appearance of moving the colors down the led strip 
                    // (it is incremented when C.advanceColor is called)
                    // Start is the offset to the start of active colors in colors array
                    leds[i] = C.getColor(((i % numCols) + offset) % numCols + start);
                }

                // Advance color after set duration (255 frames)
                if (i == 255) {
                    i = 0;
                    C.advanceColor();
                }
                i++;
            };

            /**
             * Fade the given color in and out
             */
            void fade(Controller &C)
            {
                Effects::fade(C, C.getColor());
            };

            /**
             * Fill and empty the LED strip from end to end
             */
            void fillEmpty(Controller &C)
            {
                Effects::fillEmpty(C, C.getColor());
            };

            /**
             * Fill and empty the LED strip from the end's to the middle
             */
            void fillEmptyMiddle(Controller &C)
            {
                Effects::fillEmptyMiddle(C, C.getColor());
            };
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
            void fill(Controller &C)
            {
                fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
            };

            /**
             * Fill and empty the LED strip with a rainbow gradient from end to end
             */
            void fillEmpty(Controller &C)
            {
                i = clamp(i, 0, C.getNumLEDs() * 2);
                clear(C);

                int start;
                int length;
                int hChange = 255 / C.getNumLEDs();

                //Calculate appropriate start and length depending on whether we are past the max led number
                if (i > C.getNumLEDs())
                {
                    start = i - C.getNumLEDs();
                    length = C.getNumLEDs() - (i - C.getNumLEDs());
                } else {
                    start = 0;
                    length = i;
                }
                fill_rainbow(C.getLEDs() + start, length, hChange * start, hChange);

                if (reverse) i--; else i++;
                if (i == C.getNumLEDs() * 2 || i == 0) reverse = !reverse;
            };

            /**
             * Fill the LED strip with a hue.
             * Advance hue by 1 each frame, showly shifting through all colors
             */
            void cycle(Controller &C)
            {
                CHSV hueCol(startHue, 255, 255);
                fill_solid(C.getLEDs(), C.getNumLEDs(), hueCol);

                //Iterate the hue value once for each function call.
                //Don't need to use REVERSE_HANDLER here as the hue value will just overflow back round to 0.
                startHue++;
            };

            /**
             * Fill LED strip with rainbow gradient
             * Rotate rainbow gradient by 1 each frame
             */
            void spinCycle(Controller &C)
            {
                //Functions in a similar way to the normal cycle function
                fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
                startHue++;
            };
        } // namespace Rainbow

        namespace Random
        {
            static CHSV color = CHSV(random8(), 255, 255);
            /**
             * Randomise current Random color
             */
            void randomise()
            {
                color = CHSV(random8(), 255, 255);
            };

            /**
             * Fill led strip with random color
             */
            void fill(Controller &C)
            {
                i = clamp(i, 0, 255);
                fill_solid(C.getLEDs(), C.getNumLEDs(), color);
                
                // Advance color after set duration (255 frames)
                if (i == 255) {
                    i = 0;
                    randomise();
                }
                i++;
            };

            /**
             * Fade in random color, then fade it out
             */
            void fade(Controller &C)
            {
                Effects::fade(C, color);
            };

            /**
             * Fill with random color, then empty
             */
            void fillEmpty(Controller &C)
            {
                Effects::fillEmpty(C, color);
            };

            /**
             * Fill with random color from edges, then empty to middle
             */
            void fillEmptyMiddle(Controller &C)
            {
                Effects::fillEmptyMiddle(C, color);
            };
        } // namespace Random
        
    }; // namespace Effects
};

#endif
