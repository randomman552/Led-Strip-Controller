#ifndef SerialStripController_h
#define SerialStripController_h

#include <SerialCommands.h>
#include "LEDStripController.h"


namespace LEDStripController {
    class ControllerSerialCommands : public SerialCommands {
    private:
        Controller *_parent;
    public:
        ControllerSerialCommands(Controller* parent, Stream* serial, char* buffer, int16_t buffer_len, char* term = "\r\n", char* delim = " "):
        SerialCommands(serial, buffer, buffer_len, term, delim) {
            _parent = parent;
        };

        Controller *getParent() {
            return _parent;
        };
    };

    /**
     * Subclass of Controller that takes arguments over a Serial stream,
     * Takes a stream as a constructor argument.
     * If none is provided, will default to the standard Serial stream.
     */
    class SerialController : public Controller {
    private:
        ControllerSerialCommands _commandHandler;
        char _commandBuffer[64];
    public:
        SerialController(Stream *stream);
        SerialController();
        ~SerialController();

        void mainloop();
    };
    
    /**
     * Anonymous namespace containing functions and commands for our use of the SerialCommands library.
     * Stored in an anonymous namespace to prevent access from outside this library.
     */
    namespace
    {
        namespace commandFuncs
        {
            /**
             * Unrecognised command handler
             */
            void unrecognised(SerialCommands *sender, const char *cmd);
            
            /**
             * Command handler
             * "effect/e <id(0-effects.size())>"
             */
            void effect(SerialCommands *sender);
        
            /**
             * Command handler
             * "toggle/t <state(1,0)>"
             */
            void toggle(SerialCommands *sender);
            
            /**
             * Command handler
             * "color/c" - Get current color
             * "color/c <index(0-7)>" - Get color with index
             * "color/c <r(0-255)> <g(0-255)> <b(0-255)>" - Set current color
             * "color/c <r(0-255)> <g(0-255)> <b(0-255)> <index(0-7)>" Set color with index
             */
            void editColor(SerialCommands *sender);

            /**
             * Command handler
             * "mincolor/mic <index(0-7)>"
             */
            void minColor(SerialCommands *sender);

            /**
             * Command handler
             * "maxcolor/mac <index(0-7)>"
             */
            void maxColor(SerialCommands *sender);

            /**
             * Command handler
             * "bright/b <value(0-255)>"
             */
            void brightness(SerialCommands *sender);

            /**
             * Command handler
             * "fps <value(0-255>"
             */
            void fps(SerialCommands *sender);
            
            /**
             * Command handler
             * "help/?"
             */
            void help(SerialCommands *sender);

        }; // namespace commandFuncs
    }; // namespace
};

#endif
