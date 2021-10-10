#ifndef CustomSerialCommands_h
#define CustomSerialCommands_h

#include <SerialCommands.h>

#include "StripController.h"

class ControllerSerialCommands : public SerialCommands {
private:
    StripController *_parent;
public:
    ControllerSerialCommands(StripController* parent, Stream* serial, char* buffer, int16_t buffer_len, char* term = "\r\n", char* delim = " "):
    SerialCommands(serial, buffer, buffer_len, term, delim) {
        _parent = parent;
    };

    StripController *getParent() {
        return _parent;
    }
};

#endif