#include "./Effects.h"

void Effects::clear(Controller &C)
{
    CRGB col(0, 0, 0);
    fill_solid(C.getLEDs(), C.getNumLEDs(), col);
}

int Effects::clamp(int val, int max, int min)
{
    if (val > max) return max;
    else if (val < min) return min;
    return val;
}

#pragma region Custom color lighting functions

void Effects::Color::fill(Controller &C)
{
    fill_solid(C.getLEDs(), C.getNumLEDs(), C.getColor());
}

void Effects::Color::fade(Controller &C)
{
    // Ensure i is within range
    i = clamp(i, 255, 0);

    CRGB color = C.getColor();

    color.r = color.r * ((float)i / 255);
    color.g = color.g * ((float)i / 255);
    color.b = color.b * ((float)i / 255);

    fill_solid(C.getLEDs(), C.getNumLEDs(), color);
    
    // Iterate i depending on reverse boolean
    if (reverse) i--; else i++;
    // If i has reached a limit, invert reverse and clamp i
    if (i == 255 || i == 0) reverse = !reverse;
}

void Effects::Color::fillEmpty(Controller &C)
{
    i = clamp(i, C.getNumLEDs() * 2, 0);
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
    fill_solid(C.getLEDs() + start, length, C.getColor());

    if (reverse) i--; else i++;
    if (i == C.getNumLEDs() * 2 || i == 0) reverse = !reverse;
}

void Effects::Color::fillEmptyMiddle(Controller &C)
{
    i = clamp(i, C.getNumLEDs(), 0);
    clear(C);

    int mid = C.getNumLEDs() / 2;

    //Fill the led strip appropriately depending on our iteration status
    if (i > mid) {
        //Filling from edges
        fill_solid(C.getLEDs() + i - mid, C.getNumLEDs() - (i - mid) * 2, C.getColor());
    } else {
        //Emptying to middle
        fill_solid(C.getLEDs(), i, C.getColor());
        fill_solid(C.getLEDs() + (C.getNumLEDs() - i), i, C.getColor());
    }
    
    if (reverse) i--; else i++;
    if (i == C.getNumLEDs() || i == 0) reverse = !reverse;
}

#pragma endregion

#pragma region Rainbow lighting functions

void Effects::Rainbow::fill(Controller &C)
{
    fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
}

void Effects::Rainbow::fillEmpty(Controller &C)
{
    i = clamp(i, C.getNumLEDs() * 2, 0);
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

void Effects::Rainbow::cycle(Controller &C)
{
    CHSV hueCol(startHue, 255, 255);
    fill_solid(C.getLEDs(), C.getNumLEDs(), hueCol);

    //Iterate the hue value once for each function call.
    //Don't need to use REVERSE_HANDLER here as the hue value will just overflow back round to 0.
    startHue++;
}

void Effects::Rainbow::spinCycle(Controller &C)
{
    //Functions in a similar way to the normal cycle function
    fill_rainbow(C.getLEDs(), C.getNumLEDs(), startHue, 255 / C.getNumLEDs());
    startHue++;
}

#pragma endregion
