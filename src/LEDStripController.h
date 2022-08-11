#ifndef LEDStripController_h
#define LEDStripController_h

#include <EEPROM.h>
#include <FastLED.h>
#include <LinkedList.h>

namespace LEDStripController
{
    /**
     * Clamp helper function.
     * Clamps given integer to be within the given range (inclusive)
     * i - Variable to clamp
     * max - Maximum value
     * min - Minimum value
     */
    int clamp(int val, int min, int max);

    const int version = 1;
    const int maxColors = 8;

    /**
     * Namespace containing EEPROM addresses
     */
    namespace Addrs
    {
        const int version = 0;
        const int effect = 1;
        const int brightness = 2;
        const int enabled = 3;
        const int currentColorIdx = 4;
        const int finalColorIdx = 5;
        const int fps = 6;
        const int colors = 7;
        const int end = colors + sizeof(CRGB) * maxColors;
    };

    class Controller
    {
    private:
        CRGB *_leds;
        int _numLEDs;
        int _colOffset;
    public:
        LinkedList<void (*)(Controller&)> effects;

        Controller();
        ~Controller();

        #pragma region Setters

        void setLEDs(CRGB *leds, int numLEDs);
        void setEffect(uint8_t val);
        void setBrightness(uint8_t val);
        void setEnabled(bool val);

        void setColor(CRGB val);
        void setColor(uint8_t r, uint8_t g, uint8_t b);
        void setColor(CRGB val, int idx);
        void setColor(uint8_t r, uint8_t g, uint8_t b, int idx);

        void setFPS(uint8_t val);
        void setCurrentColorIndex(uint8_t val);
        void setFinalColorIndex(uint8_t val);
        void setColorOffset(int val);

        #pragma endregion

        #pragma region Getters

        CRGB* getLEDs();
        int getNumLEDs();
        uint8_t getEffect();
        uint8_t getBrightness();
        bool getEnabled();
        CRGB getColor();
        CRGB getColor(int idx);
        uint8_t getFPS();
        uint8_t getCurrentColorIndex();
        uint8_t getFinalColorIndex();
        int getColorOffset();

        #pragma endregion

        #pragma region Other interaction functions
        
        /**
         * Main loop method of controller
         * Reads serial for commands, and draws a frame to the LEDs
         */
        void mainloop();

        /**
         * Advance the current color
         * Automatically wraps around color index
         */
        void advanceColor();

        #pragma endregion
    };
};

#include "SerialController/SerialController.h"

#endif
