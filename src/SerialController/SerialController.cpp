#include "SerialController.h"

#define arrayLength(array) sizeof(array) / sizeof(array[0])

namespace LEDStripController {
    #pragma region Constructors/destructors

    SerialController::SerialController(Stream *stream):
        _commandHandler(this, stream, _commandBuffer, sizeof _commandBuffer),
        Controller()
    {
        // Setup command handler
        _commandHandler.SetDefaultHandler(commandFuncs::unrecognised);

        _commandHandler.AddCommand(new SerialCommand("toggle", commandFuncs::toggle));
        _commandHandler.AddCommand(new SerialCommand("t", commandFuncs::toggle));
        _commandHandler.AddCommand(new SerialCommand("bright", commandFuncs::brightness));
        _commandHandler.AddCommand(new SerialCommand("effect", commandFuncs::effect));
        _commandHandler.AddCommand(new SerialCommand("getcol", commandFuncs::getColor));
        _commandHandler.AddCommand(new SerialCommand("col", commandFuncs::editColor));
        _commandHandler.AddCommand(new SerialCommand("curcol", commandFuncs::switchColor));
        _commandHandler.AddCommand(new SerialCommand("finalcol", commandFuncs::finalColor));
        _commandHandler.AddCommand(new SerialCommand("fps", commandFuncs::fps));
        // Help is aliased to "?" and "help"
        _commandHandler.AddCommand(new SerialCommand("?", commandFuncs::help));
        _commandHandler.AddCommand(new SerialCommand("help", commandFuncs::help));
    }

    SerialController::SerialController(): SerialController(&Serial) {}

    SerialController::~SerialController() {}

    #pragma endregion

    #pragma region Command handler functions

    /**
     * Utility function used to get our controller instance from the given SerialCommands object.
     */
    Controller* getController(SerialCommands *sender)  
    {
        return ((ControllerSerialCommands*) sender)->getParent();
    }

    void commandFuncs::unrecognised(SerialCommands *sender, const char *cmd) 
    {
        sender->GetSerial()->print("ERROR: '");
        sender->GetSerial()->print(cmd);
        sender->GetSerial()->println("' IS NOT RECOGNISED");
    }

    void commandFuncs::fps(SerialCommands *sender)
    {
        char *input = sender->Next();

        // Set new value if provided
        if (strlen(input) != 0) {
            uint8_t val = atoi(input);

            // Make sure value is in range
            if (val < 1 || val > 255) {
                sender->GetSerial()->println("ERROR: Value must be in range 1-255");
                return;
            }
            getController(sender)->setFps(val);
            sender->GetSerial()->println("OK");
            return;
        }

        // When no value specified, show current value
        sender->GetSerial()->println(getController(sender)->getFps());
    }

    void commandFuncs::brightness(SerialCommands *sender) 
    {
        char *input = sender->Next();
        uint8_t newVal = atoi(input);

        //Check value is valid
        if (input == NULL || newVal > 255){
            sender->GetSerial()->println(FastLED.getBrightness());
            return;
        }
        getController(sender)->setBrightness(newVal);
        sender->GetSerial()->println("OK");
    }

    void commandFuncs::editColor(SerialCommands *sender)
    {
        char *rInp = sender->Next();
        char *gInp = sender->Next();
        char *bInp = sender->Next();
        char *idxInp = sender->Next();

        // Get color at specified idx
        int idx = getController(sender)->getCurColIdx();
        if (strlen(idxInp) != 0) {
            idx = atoi(idxInp);
            if (idx < 0 || idx >= maxColors) {
                sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
                sender->GetSerial()->println(maxColors - 1);
                return;
            }
        }
        CRGB col = getController(sender)->getColor(idx);

        // Check inputs are valid
        if (strlen(rInp) == 0 || strlen(gInp) == 0 || strlen(bInp) == 0) {
            sender->GetSerial()->println("ERROR: Must specify R, G, and B values");
            return;
        }

        col.r = atoi(rInp);
        col.g = atoi(gInp);
        col.b = atoi(bInp);

        getController(sender)->setColor(col, idx);

        sender->GetSerial()->println("OK");
    }

    void commandFuncs::getColor(SerialCommands *sender) 
    {
        char *idxInp = sender->Next();

        int idx = getController(sender)->getCurColIdx();
        if (strlen(idxInp) != 0) {
            idx = atoi(idxInp);
            // Check if index is within range
            if (idx < 0 || idx >= maxColors) {
                sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
                sender->GetSerial()->println(maxColors - 1);
                return;
            }
        }
        CRGB curCol = getController(sender)->getColor(idx);

        sender->GetSerial()->print(curCol.r);
        sender->GetSerial()->print(", ");
        sender->GetSerial()->print(curCol.g);
        sender->GetSerial()->print(", ");
        sender->GetSerial()->println(curCol.b);
    }

    void commandFuncs::switchColor(SerialCommands *sender) 
    {
        char *input = sender->Next();
        char newVal = atoi(input);

        // If no number provided, echo current value
        if (strlen(input) == 0) {
            sender->GetSerial()->println(getController(sender)->getCurColIdx());
            return;
        }

        // Check given value in range
        if (newVal < 0 || newVal >= maxColors) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(maxColors - 1);
            return;
        }

        // Update value
        getController(sender)->setCurColIdx(newVal);
        // Reset current offset to prevent any out of range errors
        getController(sender)->setOffset(0);
        sender->GetSerial()->println("OK");
    }

    void commandFuncs::effect(SerialCommands *sender) 
    {   
        char *input = sender->Next();
        int newVal = atoi(input);

        // If new value is null report current value
        if (strlen(input) == 0) {
            sender->GetSerial()->println(getController(sender)->getEffect());
            return;
        }

        // If new value is out of range, display error
        if (!(newVal <= arrayLength(LEDStripController::lFuncs) - 1 && newVal >= 0)) {
            sender->GetSerial()->print("ERROR: Effect must be in range 0 - ");
            sender->GetSerial()->println(arrayLength(LEDStripController::lFuncs) - 1);
            return;
        }

        getController(sender)->setEffect(newVal);
        sender->GetSerial()->println("OK");
    }

    void commandFuncs::toggle(SerialCommands *sender)
    {
        getController(sender)->setEnabled(!getController(sender)->getEnabled());
        sender->GetSerial()->println("OK");
    }

    void commandFuncs::help(SerialCommands *sender) 
    {
        sender->GetSerial()->println("https://github.com/randomman552/Led-Strip-Controller");
    }

    void commandFuncs::finalColor(SerialCommands *sender)
    {
        char *input = sender->Next();
        int newVal = atoi(input);

        if (strlen(input) == 0) {
            sender->GetSerial()->println(getController(sender)->getFinColIdx());
            return;
        }

        if (newVal < 0 || newVal >= maxColors) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(maxColors - 1);
            return;
        }
        getController(sender)->setFinColIdx(newVal);
        getController(sender)->setOffset(0);
        sender->GetSerial()->println("OK");
    }

    #pragma endregion

    #pragma region Method overrides

    void SerialController::mainloop()
    {
        _commandHandler.ReadSerial();
        Controller::mainloop();
    }

#pragma endregion
};
