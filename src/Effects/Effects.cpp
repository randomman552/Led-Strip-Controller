#include "Effects.h"

int clamp(int val, int min, int max)
{
    if (val > max) return max;
    else if (val < min) return min;
    return val;
}

#pragma region Base functions

void Effects::clear(StripController &C)
{
    CRGB col(0, 0, 0);
    fill_solid(C.getLEDs(), C.getNumLEDs(), col);
}


void Effects::fade(StripController &C, CRGB col)
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
}

void Effects::fade(StripController &C, CHSV col)
{
    CRGB rgbCol;
    hsv2rgb_rainbow(col, rgbCol);
    fade(C, rgbCol);
}


void Effects::fillEmpty(StripController &C, CRGB col)
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
}

void Effects::fillEmpty(StripController &C, CHSV col)
{
    CRGB rgbCol;
    hsv2rgb_rainbow(col, rgbCol);
    fillEmpty(C, rgbCol);
}


void Effects::fillEmptyMiddle(StripController &C, CRGB col)
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
}

void Effects::fillEmptyMiddle(StripController &C, CHSV col)
{
    CRGB rgbCol;
    hsv2rgb_rainbow(col, rgbCol);
    fillEmptyMiddle(C, rgbCol);
}

#pragma endregion

#pragma region Custom color lighting functions

void Effects::Color::fill(StripController &C)
{   
    i = clamp(i, 0, 255);
    fill_solid(C.getLEDs(), C.getNumLEDs(), C.getColor());
    
    // Advance color after set duration (255 frames)
    if (i == 255) {
        i = 0;
        C.advanceColor();
    }
    i++;
}

void Effects::Color::alternateFill(StripController &C)
{
    int start = C.getCurColIdx();
    int numCols = C.getFinColIdx() + 1 - start;
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
}

void Effects::Color::fade(StripController &C)
{
    Effects::fade(C, C.getColor());
}

void Effects::Color::fillEmpty(StripController &C)
{
    Effects::fillEmpty(C, C.getColor());
}

void Effects::Color::fillEmptyMiddle(StripController &C)
{
    Effects::fillEmptyMiddle(C, C.getColor());
}

#pragma endregion

#pragma region Rainbow lighting functions

void Effects::Rainbow::fill(StripController &C)
{
    fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
}

void Effects::Rainbow::fillEmpty(StripController &C)
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
}

void Effects::Rainbow::cycle(StripController &C)
{
    CHSV hueCol(startHue, 255, 255);
    fill_solid(C.getLEDs(), C.getNumLEDs(), hueCol);

    //Iterate the hue value once for each function call.
    //Don't need to use REVERSE_HANDLER here as the hue value will just overflow back round to 0.
    startHue++;
}

void Effects::Rainbow::spinCycle(StripController &C)
{
    //Functions in a similar way to the normal cycle function
    fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
    startHue++;
}

#pragma endregion

#pragma region Random lighting functions

void Effects::Random::randomise()
{
    color = CHSV(random8(), 255, 255);
}

void Effects::Random::fill(StripController &C)
{
    i = clamp(i, 0, 255);
    fill_solid(C.getLEDs(), C.getNumLEDs(), color);
    
    // Advance color after set duration (255 frames)
    if (i == 255) {
        i = 0;
        randomise();
    }
    i++;
}

void Effects::Random::fade(StripController &C)
{
    Effects::fade(C, color);
}

void Effects::Random::fillEmpty(StripController &C)
{
    Effects::fillEmpty(C, color);
}

void Effects::Random::fillEmptyMiddle(StripController &C)
{
    Effects::fillEmptyMiddle(C, color);
}

#pragma endregion
