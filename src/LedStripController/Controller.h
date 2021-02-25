#ifndef LEDCON_Controller_h
#define LEDCON_Controller_h

#include <SoftwareSerial.h>
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
    char _commandBuffer[32];
    SoftwareSerial _serial;
    SerialCommands _commandHandler;
    static Controller *_instance;
public:
    Controller(int rx, int tx);
    ~Controller();

    // Setters
    void setLEDs(CRGB *leds, int numLEDs);
    void setEffect(uint8_t val);
    void setBrightness(uint8_t val);
    void setEnabled(bool val);
    void setColor(CRGB val);
    void setColor(CRGB val, int idx);
    void setCurColIdx(uint8_t val);

    // Getters
    CRGB* getLEDs();
    int getNumLEDs();
    uint8_t getEffect();
    uint8_t getBrightness();
    bool getEnabled();
    CRGB getColor();
    CRGB getColor(int idx);
    uint8_t getCurColIdx();

    // Get controller instance
    static Controller *getInstance() {
        return _instance;
    };

    /**
     * Main loop method of controller
     * Reads serial for commands, and draws a frame to the LEDs
     */
    void mainloop();
};


#endif
