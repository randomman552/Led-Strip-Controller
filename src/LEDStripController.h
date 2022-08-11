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

    /**
     * The Controller class forms a wrapper around an array of LED's from the FastLED library.
     * The class then applies a series of lighting effect functions,
     * which can be customised and controlled through setters and getters on this class.
     */
    class Controller
    {
    private:
        CRGB *_leds;
        int _numLEDs;
        int _colOffset;
    public:
        /**
         * @brief List of lighting effect functions.
         * You can edit this list to control what functions are available to the Controller instance.
         * If you want to add your own lighting functions, view the README for more information.
         */
        LinkedList<void (*)(Controller&)> effects;

        Controller();
        ~Controller();

        #pragma region Setters

        /**
         * @brief Set the array of LEDs this controller instance interacts with.
         * @param leds Pointer to the array of FastLED leds created using the FastLED library.
         * @param numLEDs The number of led's in the array
         */
        void setLEDs(CRGB *leds, int numLEDs);
        /**
         * @brief Set the Effect index, 
         * this value will be used to get a lighting function from the effects linked list.
         * @param val The new value
         */
        void setEffect(uint8_t val);
        /**
         * @brief Set the Brightness value
         * @param val The desired value (between 0 and 255)
         */
        void setBrightness(uint8_t val);
        /**
         * @brief Set whether the Controller should display a lighting effect.
         * @param val Desired value
         */
        void setEnabled(bool val);

        /**
         * @brief Set the current color
         * @param val The new color value
         */
        void setColor(CRGB val);
        /**
         * @brief Set the current color
         * @param r Red value (0-255)
         * @param g Green value (0-255)
         * @param b Blue value (0-255)
         */
        void setColor(uint8_t r, uint8_t g, uint8_t b);
        /**
         * @brief Set the color with the given index
         * @param val New value
         * @param idx Index of color to set
         */
        void setColor(CRGB val, int idx);
        /**
         * @brief Set the color with the given index
         * 
         * @param r Red value (0-255)
         * @param g Green value (0-255)
         * @param b Blue value (0-255)
         * @param idx Index of the color to change
         */
        void setColor(uint8_t r, uint8_t g, uint8_t b, int idx);

        /**
         * @brief Set the maximum fps. This is used to control the speed of animation
         * @param val new value
         */
        void setFPS(uint8_t val);
        
        /**
         * @brief Set the Current Color Index (minimum)
         * @param val New value
         */
        void setMinimumColorIndex(uint8_t val);
        /**
         * @brief Set the Final Color Index (maximum)
         * @param val New value
         */
        void setMaximumColorIndex(uint8_t val);
        /**
         * @brief Set the Current Color Offset
         * The Controller supports up to 8 colors to be set at once.
         * A Controller object will automatically cycle between the minimum and maximum color index
         * (assuming this functionality is supported by any lighting effect functions it utilises).
         * The Color Offset is the index offset applied from the minimum for which the corresponding color is retrieved.
         * @param val New value
         */
        void setColorIndexOffset(int val);

        #pragma endregion

        #pragma region Getters

        /**
         * @brief Get the LED's array
         * @return CRGB* Pointer to the array of LED's controlled by this Controller instance
         */
        CRGB* getLEDs();
        /**
         * @brief Get the number of LEDs
         * @return int 
         */
        int getNumLEDs();
        /**
         * @brief Get the current Effect index
         * @return uint8_t 
         */
        uint8_t getEffect();
        /**
         * @brief Get the current Brightness
         * @return uint8_t 
         */
        uint8_t getBrightness();
        /**
         * @brief Get the Enabled boolean
         * @return true This Controller instance is enabled.
         * @return false This Controller instance is disabled.
         */
        bool getEnabled();
        /**
         * @brief Get the Current Color object
         * @return CRGB 
         */
        CRGB getColor();
        /**
         * @brief Get the Color object with the given index
         * @param idx Index of color to fetch
         * @return CRGB 
         */
        CRGB getColor(int idx);
        /**
         * @brief Get the current maximum FPS value
         * @return uint8_t 
         */
        uint8_t getFPS();
        
        /**
         * @brief Get the Current Color Index (minimum)
         * @return uint8_t 
         */
        uint8_t getMinimumColorIndex();
        /**
         * @brief Get the Final Color Index (maximum)
         * @return uint8_t 
         */
        uint8_t getMaximumColorIndex();
        /**
         * @brief Get the Color Offset
         * This offset should be applied to the Current Color Index (minimum)
         * @return int 
         */
        int getColorIndexOffset();

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

#include "SerialController.h"

#endif
