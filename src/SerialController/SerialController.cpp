#include "SerialController.h"

namespace LEDStripController {
    #pragma region Constructors/destructors

    SerialController::SerialController(Stream *stream):
        _commandHandler(this, stream, _commandBuffer, sizeof _commandBuffer),
        Controller()
    {
        // Setup command handler
        _commandHandler.SetDefaultHandler(commandFuncs::unrecognised);

        // Help is aliased to "?" and "help"
        _commandHandler.AddCommand(new SerialCommand("?", commandFuncs::help));
        _commandHandler.AddCommand(new SerialCommand("help", commandFuncs::help));

        // Toggle is aliased to "toggle" and "t"
        _commandHandler.AddCommand(new SerialCommand("toggle", commandFuncs::toggle));
        _commandHandler.AddCommand(new SerialCommand("t", commandFuncs::toggle));

        // Brightness is aliased to "bright" and "b"
        _commandHandler.AddCommand(new SerialCommand("bright", commandFuncs::brightness));
        _commandHandler.AddCommand(new SerialCommand("b", commandFuncs::brightness));

        // Effect is aliased to "effect" and "e"
        _commandHandler.AddCommand(new SerialCommand("effect", commandFuncs::effect));
        _commandHandler.AddCommand(new SerialCommand("e", commandFuncs::effect));

        // Color is aliased to "color" and "c"
        _commandHandler.AddCommand(new SerialCommand("col", commandFuncs::editColor));
        _commandHandler.AddCommand(new SerialCommand("c", commandFuncs::editColor));

        // Current color is aliased to "mincolor" and "mic"
        _commandHandler.AddCommand(new SerialCommand("mincolor", commandFuncs::minColor));
        _commandHandler.AddCommand(new SerialCommand("mic", commandFuncs::minColor));

        // Final color is aliased to "maxcolor" and "mac"
        _commandHandler.AddCommand(new SerialCommand("maxcolor", commandFuncs::maxColor));
        _commandHandler.AddCommand(new SerialCommand("mac", commandFuncs::maxColor));

        // FPS is not aliased as it can't be shortened further
        _commandHandler.AddCommand(new SerialCommand("fps", commandFuncs::fps));
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
            getController(sender)->setFPS(val);
            sender->GetSerial()->println("OK");
            return;
        }

        // When no value specified, show current value
        sender->GetSerial()->println(getController(sender)->getFPS());
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
        char *input1 = sender->Next();
        char *input2 = sender->Next();
        char *input3 = sender->Next();
        char *input4 = sender->Next();
        Controller* controller = getController(sender);

        // Get number of arguments provided
        int numArgs = ((strlen(input1) > 0) + (strlen(input2) > 0) + (strlen (input3) > 0) + (strlen(input4) > 0));

        // Define variables
        uint8_t r, g, b, i;

        
        switch (numArgs)
        {
        case 0:
            // If no arguments provided, return the current color
            sender->GetSerial()->print(controller->getColor().r);
            sender->GetSerial()->print(", ");
            sender->GetSerial()->print(controller->getColor().g);
            sender->GetSerial()->print(", ");
            sender->GetSerial()->println(controller->getColor().b);
            break;

        case 1:
            // If one argument provided, return the color at the given position
            i = atoi(input1);
            sender->GetSerial()->print(controller->getColor(i).r);
            sender->GetSerial()->print(", ");
            sender->GetSerial()->print(controller->getColor(i).g);
            sender->GetSerial()->print(", ");
            sender->GetSerial()->println(controller->getColor(i).b);
            break;

        case 3:
            // If three arguments provided, we change the current color
            r = atoi(input1);
            g = atoi(input2);
            b = atoi(input3);

            controller->setColor(r, g, b);
            sender->GetSerial()->println("OK");
            break;

        case 4:
            // If four arguments provided, we change the color with the given index
            r = atoi(input1);
            g = atoi(input2);
            b = atoi(input3);
            i = atoi(input4);

            // Check index bounds
            if (i < 0 || i >= maxColors) {
                sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
                sender->GetSerial()->println(maxColors - 1);
                break;
            }

            controller->setColor(r, g, b, i);
            sender->GetSerial()->println("OK");
            break;

        default:
            sender->GetSerial()->println("ERROR: Invalid number of arguments provided");
            break;
        }
    }

    void commandFuncs::minColor(SerialCommands *sender) 
    {
        char *input = sender->Next();
        char newVal = atoi(input);

        // If no number provided, echo current value
        if (strlen(input) == 0) {
            sender->GetSerial()->println(getController(sender)->getMinimumColorIndex());
            return;
        }

        // Check given value in range
        if (newVal < 0 || newVal >= maxColors) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(maxColors - 1);
            return;
        }

        // Update value
        getController(sender)->setMinimumColorIndex(newVal);
        // Reset current offset to prevent any out of range errors
        getController(sender)->setColorIndexOffset(0);
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
        if (!(newVal <= getController(sender)->effects.size() - 1 && newVal >= 0)) {
            sender->GetSerial()->print("ERROR: Effect must be in range 0 - ");
            sender->GetSerial()->println(getController(sender)->effects.size() - 1);
            return;
        }

        getController(sender)->setEffect(newVal);
        sender->GetSerial()->println("OK");
    }

    void commandFuncs::toggle(SerialCommands *sender)
    {
        Controller* c = getController(sender);
        char *arg = sender->Next();

        if (strlen(arg) > 0) {
            int state = atoi(arg);
            c->setEnabled((bool)state);
        } else {
            // Flip the state if no argument provided
            c->setEnabled(!c->getEnabled());
        }

        // Inform the user of current state
        if (c->getEnabled()) {
            sender->GetSerial()->println("ON");
            return;
        }
        sender->GetSerial()->println("OFF");
    }

    void commandFuncs::help(SerialCommands *sender) 
    {
        sender->GetSerial()->println("https://github.com/randomman552/Led-Strip-Controller");
    }

    void commandFuncs::maxColor(SerialCommands *sender)
    {
        char *input = sender->Next();
        int newVal = atoi(input);

        if (strlen(input) == 0) {
            sender->GetSerial()->println(getController(sender)->getMaximumColorIndex());
            return;
        }

        if (newVal < 0 || newVal >= maxColors) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(maxColors - 1);
            return;
        }
        getController(sender)->setMaximumColorIndex(newVal);
        getController(sender)->setColorIndexOffset(0);
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
