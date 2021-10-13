#ifndef SerialStripController_h
#define SerialStripController_h

#include "SerialCommands.h"


namespace LEDStripController {
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
        }; // namespace commandFuncs
    }; // namespace
    
    /**
     * Subclass of Controller that takes arguments over a Serial stream,
     * Takes a stream as a constructor argument.
     * If none is provided, will default to the standard Serial stream.
     */
    class SerialController : public Controller {
    private:
        ControllerSerialCommands _commandHandler;
        char _commandBuffer[32];
    public:
        SerialController(Stream *stream);
        SerialController();
        ~SerialController();

        void mainloop();
    };
};

#endif
