#ifndef LEDStripController_h
#define LEDStripController_h

#include <EEPROM.h>
#include <FastLED.h>

namespace LEDStripController
{
    const int version = 3;
    const int maxColors = 8;

    /**
     * Namespace containing EEPROM addresses
     */
    namespace Addrs
    {
        const int version = 1;
        const int effect = 2;
        const int brightness = 3;
        const int enabled = 4;
        const int currentColorIdx = 5;
        const int finalColorIdx = 6;
        const int colors = 7;
        const int end = 7 + sizeof(CRGB) * maxColors;
    };

    class Controller
    {
    private:
        CRGB *_leds;
        int _numLEDs;
        int _colOffset;
    public:
        Controller();
        ~Controller();

        #pragma region Setters

        void setLEDs(CRGB *leds, int numLEDs);
        void setEffect(uint8_t val);
        void setBrightness(uint8_t val);
        void setEnabled(bool val);
        void setColor(CRGB val);
        void setColor(CRGB val, int idx);
        void setCurColIdx(uint8_t val);
        void setFinColIdx(uint8_t val);
        void setOffset(int val);

        #pragma endregion

        #pragma region Getters

        CRGB* getLEDs();
        int getNumLEDs();
        uint8_t getEffect();
        uint8_t getBrightness();
        bool getEnabled();
        CRGB getColor();
        CRGB getColor(int idx);
        uint8_t getCurColIdx();
        uint8_t getFinColIdx();
        int getColOffset();

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

    extern void (*lFuncs[13])(Controller&);
};

#include "SerialController/SerialController.h"

#endif
