#ifndef CustomSerialCommands_h
#define CustomSerialCommands_h

#include <SerialCommands.h>
#include "../LEDStripController.h"

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
};

#endif
