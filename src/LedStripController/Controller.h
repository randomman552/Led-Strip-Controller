#ifndef LEDCON_Controller_h
#define LEDCON_Controller_h

#include <EEPROM.h>
#include <SerialCommands.h>
#include <FastLED.h>

class Controller;
#include "./Effects/Effects.h"

/**
 * Anonymous namespace containing functions and commands for our use of the SerialCommands library
 */
namespace
{
    namespace commandFuncs
    {
        /**
         * Tells the user that the command they entered is not recognised
         */
        void unrecognised(SerialCommands *sender, const char *cmd);
        
        /**
         * Effect command handler
         * Form: "effect <effect id(0-9)>"
         */
        void effect(SerialCommands *sender);
       
        /**
         * Toggle command handler
         * Form: "toggle"
         */
        void toggle(SerialCommands *sender);

        /**
         * Color getting command handler
         * Form: "getColor <idx(0-MAX_COLORS)>"
         */
        void getColor(SerialCommands *sender);
        
        /**
         * Color editing command handler
         * Form: "editColor <red(0-255)> <green(0-255)> <blue(0-255)> <Optional:idx(0-MAX_COLORS-1)> "
         */
        void editColor(SerialCommands *sender);

        /**
         * Color switching command handler
         * Form: switchColor <idx(0-MAX_COLORS)>
         */
        void switchColor(SerialCommands *sender);

        /**
         * Brightness command handler
         * Form: "brightness <value(0-255)>"
         */
        void brightness(SerialCommands *sender);
        
        /**
         * Function to print out re-direct to GitHub repo, where a list of commands is provided in README.md
         * I have done this instead of adding onboard help in an attempt to reduce memory consumption.
         * I may later add onboard help using PROGMEM to use program storage space
         */
        void help(SerialCommands *sender);

        /**
         * Set the final color to be used in color cycle
         */
        void finalColor(SerialCommands *sender);
    } // namespace commandFuncs
}; // namespace


/**
 * Singleton Controller class
 * Handles controlling of an LED strip, and recieves commands on the defined RX and TX channels
 * Call Controller::getInstance() to get the controller instance if already created, otherwise use the constructor
 */
class Controller
{
private:
    CRGB *_leds;
    int _numLEDs;
    /**
     * Buffer for use with command handler
     */
    char _commandBuffer[32];
    /**
     * SerialCommands object to handle commands recieved over _serial
     */
    SerialCommands _commandHandler;
    /**
     * Pointer to singleton Controller instance
     */
    static Controller *_instance;
    // Offset to get for current color
    int _colOffset;
public:
    Controller(Stream *serial);
    ~Controller();

    // Setters
    void setLEDs(CRGB *leds, int numLEDs);
    void setEffect(uint8_t val);
    void setBrightness(uint8_t val);
    void setEnabled(bool val);
    void setColor(CRGB val);
    void setColor(CRGB val, int idx);
    void setCurColIdx(uint8_t val);
    void setFinColIdx(uint8_t val);
    void setOffset(int val);

    // Getters
    CRGB* getLEDs();
    int getNumLEDs();
    uint8_t getEffect();
    uint8_t getBrightness();
    bool getEnabled();
    // Current color
    CRGB getColor();
    // Color with the given index
    CRGB getColor(int idx);
    // Index of the current color
    uint8_t getCurColIdx();
    // Index of the final active color
    uint8_t getFinColIdx();
    // Current color offset (specifies first color to use in color array)
    int getOffset();

    // Get controller instance
    static Controller *getInstance() {
        return _instance;
    };

    // External interaction functions
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
};


#endif
