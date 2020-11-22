#include "Effects.h"


/**
 * Macro to handle the various reverse conditions used within different lighting functions.
 * condition - The condition that will cause the reverse value to flip.
 * i - The iterator to increment or decrement based in the revVar.
 * revVar - The boolean controlling whether to increment or decrement i.
 */
#define REVERSE_HANDLER(condition, i, revVar) if (revVar) { i--; } else { i++; }; if (condition) { reverse = !reverse; }

int Effects::numLEDs = 0;
CRGB *Effects::leds;
CRGB Effects::color(255, 255, 255);


void Effects::clear()
{
    CRGB color(0, 0, 0);
    fill_solid(leds, numLEDs, color);
}


#pragma region Custom color functions

void Effects::CustomColor::fill()
{
    fill_solid(leds, numLEDs, color);
}

void Effects::CustomColor::fade()
{
    //Current iteration status is static, so will be stored between function calls.
    static uint8_t i = 0;
    static bool reverse = false;

    float rShift = (float)color.r / 255;
    float gShift = (float)color.g / 255;
    float bShift = (float)color.b / 255;

    //Calculate the color we need to apply and apply it to the leds
    CRGB color(rShift * (float)i, gShift * (float)i, bShift * (float)i);
    fill_solid(leds, numLEDs, color);

    REVERSE_HANDLER(i == 255 || i == 0, i, reverse);
}

void Effects::CustomColor::fillEmpty()
{
    //Static variables to store states between function calls.
    static int i = 0;
    static bool reverse = false;
    
    clear();


    int start;
    int length;

    //Calculate appropriate start and length depending on whether we are past the max led number
    if (i > numLEDs)
    {
        start = i - numLEDs;
        length = numLEDs - (i - numLEDs);
    } else {
        start = 0;
        length = i;
    }
    fill_solid(leds + start, length, color);

    REVERSE_HANDLER(i == numLEDs * 2 || i == 0, i, reverse);
}

void Effects::CustomColor::fillEmptyMiddle()
{
    //Static variables to store states between function calls.
    static int i = 0;
    static bool reverse = false;

    clear();

    int mid = numLEDs / 2;

    //Fill the led strip appropriately depending on our iteration status
    if (i > mid) {
        //Filling from edges
        fill_solid(leds + i - mid, numLEDs - (i - mid) * 2, color);
    } else {
        //Emptying to middle
        fill_solid(leds, i, color);
        fill_solid(leds + (numLEDs - i), i, color);
    }
    
    REVERSE_HANDLER(i == numLEDs || i == 0, i, reverse);
}

void Effects::CustomColor::bounce()
{
    static int i = 0;
    static bool reverse = false;

    clear();

    leds[i] = color;

    REVERSE_HANDLER(i == numLEDs || i == 0, i, reverse)
}

#pragma endregion

#pragma region Rainbow color functions

void Effects::Rainbow::fill()
{
    int hChange = 255 / numLEDs;
    fill_rainbow(leds, numLEDs, 0, hChange);
}

void Effects::Rainbow::fillEmpty()
{
    //Static variables to store states between function calls.
    static int i = 0;
    static bool reverse = false;
    
    clear();


    int start;
    int length;
    int hChange = 255 / numLEDs;

    //Calculate appropriate start and length depending on whether we are past the max led number
    if (i > numLEDs)
    {
        start = i - numLEDs;
        length = numLEDs - (i - numLEDs);
    } else {
        start = 0;
        length = i;
    }
    fill_rainbow(leds + start, length, hChange * start, hChange);

    REVERSE_HANDLER(i == numLEDs * 2 || i == 0, i, reverse);
}

void Effects::Rainbow::cycle()
{
    //Static variable to store current hue between function calls.
    static uint8_t hue = 0;
    CHSV hueCol(hue, 255, 255);
    fill_solid(leds, numLEDs, hueCol);

    //Iterate the hue value once for each function call.
    //Don't need to use REVERSE_HANDLER here as the hue value will just overflow back round to 0.
    hue++;
}

void Effects::Rainbow::spinCycle() 
{
    //Functions in a similar way to the normal cycle function
    static uint8_t hue = 0;
    fill_rainbow(leds, numLEDs, hue, 255 / numLEDs);
    hue++;
}

#pragma endregion

#pragma region Random lighting functions



#pragma endregion
