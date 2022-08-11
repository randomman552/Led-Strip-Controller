#include "Effects.h"

// Base lighting functions
namespace LEDStripController::Effects {

    void clear(Controller &C) {
        CRGB col(0, 0, 0);
        fill_solid(C.getLEDs(), C.getNumLEDs(), col);
    }


    void fill(Controller &C, CRGB col) {
        i = clamp(i, 0, 255);
        fill_solid(C.getLEDs(), C.getNumLEDs(), col);
        
        // Advance color after set duration (255 frames)
        if (i == 255) {
            i = 0;
            C.advanceColor();
            Random::randomise();
        }
        i++;
    }

    void fill(Controller &C, CHSV col) {
        CRGB rgbCol;
        hsv2rgb_rainbow(col, rgbCol);
        fill(C, rgbCol);
    }


    void fade(Controller &C, CRGB col) {
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
            Random::randomise();
        }
    }

    void fade(Controller &C, CHSV col) {
        CRGB rgbCol;
        hsv2rgb_rainbow(col, rgbCol);
        fade(C, rgbCol);
    }


    void fillEmpty(Controller &C, CRGB col) {
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
            Random::randomise();
        }
    }

    void fillEmpty(Controller &C, CHSV col) {
        CRGB rgbCol;
        hsv2rgb_rainbow(col, rgbCol);
        fillEmpty(C, rgbCol);
    }


    void fillEmptyMiddle(Controller &C, CRGB col) {
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
            Random::randomise();
        }
    }

    void fillEmptyMiddle(Controller &C, CHSV col) {
        CRGB rgbCol;
        hsv2rgb_rainbow(col, rgbCol);
        fillEmptyMiddle(C, rgbCol);
    }
};

// Color lighting functions
namespace LEDStripController::Effects::Color {
    void fill(Controller &C) {
        Effects::fill(C, C.getColor());
    }

    void alternateFill(Controller &C) {
        int start = C.getMinimumColorIndex();
        int numCols = C.getMaximumColorIndex() + 1 - start;
        CRGB *leds = C.getLEDs();
        int numLEDs = C.getNumLEDs();
        int offset = C.getColorIndexOffset();

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
    }

    void fade(Controller &C) {
        Effects::fade(C, C.getColor());
    }

    void fillEmpty(Controller &C) {
        Effects::fillEmpty(C, C.getColor());
    }

    void fillEmptyMiddle(Controller &C) {
        Effects::fillEmptyMiddle(C, C.getColor());
    }
};

// Rainbow lighting functions
namespace LEDStripController::Effects::Rainbow {
    void fill(Controller &C) {
        fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
    }

    void fillEmpty(Controller &C) {
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
    }

    void cycle(Controller &C) {
        CHSV hueCol(startHue, 255, 255);
        fill_solid(C.getLEDs(), C.getNumLEDs(), hueCol);

        //Iterate the hue value once for each function call.
        //Don't need to use REVERSE_HANDLER here as the hue value will just overflow back round to 0.
        startHue++;
    }

    void spinCycle(Controller &C) {
        //Functions in a similar way to the normal cycle function
        fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
        startHue++;
    }
};

// Random lighting functions
namespace LEDStripController::Effects::Random {
    void randomise() {
        color = CHSV(random8(), 255, 255);
    }

    void fill(Controller &C) {
        Effects::fill(C, color);
    }

    void fade(Controller &C) {
        Effects::fade(C, color);
    }

    void fillEmpty(Controller &C) {
        Effects::fillEmpty(C, color);
    }

    void fillEmptyMiddle(Controller &C) {
        Effects::fillEmptyMiddle(C, color);
    }
};