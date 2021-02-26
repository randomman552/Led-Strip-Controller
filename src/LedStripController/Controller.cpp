#include "./Controller.h"

#define VERSION 3
#define MAX_COLORS 8

#define ADDR_VERSION 1
#define ADDR_EFFECT 2
#define ADDR_BRIGHTNESS 3
#define ADDR_ENABLED 4
#define ADDR_CURRENT_COLOR_IDX 5
#define ADDR_FINAL_COLOR_IDX 6
#define ADDR_COLORS 7

#define arrayLength(array) sizeof(array) / sizeof(array[0])


/**
 * Array of lighting function pointers
 * Allows us to call an lighting function by index rather than by name
 */
static void (*lFuncs[])(Controller&) = {
    Effects::Color::fill,
    Effects::Color::alternateFill,
    Effects::Color::fade,
    Effects::Color::fillEmpty,
    Effects::Color::fillEmptyMiddle,
    Effects::Rainbow::fill,
    Effects::Rainbow::fillEmpty,
    Effects::Rainbow::cycle,
    Effects::Rainbow::spinCycle,
    Effects::Random::fill,
    Effects::Random::fade,
    Effects::Random::fillEmpty,
    Effects::Random::fillEmptyMiddle
};

// Initalise _instance to be a null pointer
Controller *Controller::_instance = nullptr;

#pragma region Constructors

Controller::Controller(int rx, int tx):
    _serial(rx, tx),
    _commandHandler(&_serial, _commandBuffer, sizeof _commandBuffer)
{
    // If singleton instance has already been created, use that
    if (_instance) {
        Controller(*_instance);
        return;
    }

    // Check for EEPROM version mismatch
    if (EEPROM.read(ADDR_VERSION) != VERSION) {
        // Perform version update
        CRGB color(255, 255, 255);

        setEffect(0);
        setBrightness(64);
        setEnabled(true);
        setCurColIdx(0);
        setFinColIdx(0);
        EEPROM.update(ADDR_VERSION, VERSION);

        // Default all colors to white
        for (int i = 0; i < MAX_COLORS; i++)
        {
            setColor(color, i);
        }
    }

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
    // Help is aliased to "?" and "help"
    _commandHandler.AddCommand(new SerialCommand("?", commandFuncs::help));
    _commandHandler.AddCommand(new SerialCommand("help", commandFuncs::help));

    // Set singleton instance
    _instance = this;

    // Initalise color index offset
    _colOffset = 0;

    // Start software serial
    _serial.begin(38400);

    // Load saved values
    FastLED.setBrightness(getBrightness());
    FastLED.setMaxRefreshRate(60);
}

Controller::~Controller()
{
}

#pragma endregion

#pragma region Setters

void Controller::setLEDs(CRGB *leds, int numLEDs) { _leds = leds; _numLEDs = numLEDs; }

void Controller::setBrightness(uint8_t val) { EEPROM.update(ADDR_BRIGHTNESS, val); FastLED.setBrightness(val); }

void Controller::setEffect(uint8_t val) { EEPROM.update(ADDR_EFFECT, clamp(val, 0, arrayLength(lFuncs) - 1)); }

void Controller::setEnabled(bool val) { EEPROM.update(ADDR_ENABLED, val); }

void Controller::setColor(CRGB val) { setColor(val, getCurColIdx()); }

void Controller::setColor(CRGB val, int idx) {
    clamp(idx, 0, MAX_COLORS - 1);
    idx = ADDR_COLORS + idx * sizeof(CRGB);
    EEPROM.put<CRGB>(idx, val);
}

void Controller::setCurColIdx(uint8_t val) { 
    val = clamp(val, 0, MAX_COLORS);
    setFinColIdx(val + getFinColIdx());
    EEPROM.update(ADDR_CURRENT_COLOR_IDX, val);
}

void Controller::setFinColIdx(uint8_t val) {
    val = clamp(val, getCurColIdx(), MAX_COLORS - 1);
    EEPROM.update(ADDR_FINAL_COLOR_IDX, val); 
}

void Controller::setOffset(int val) {
    val = clamp(val, 0, getFinColIdx());
    _colOffset = val;
}

#pragma endregion

#pragma region Getters

CRGB* Controller::getLEDs() { return _leds; }

int Controller::getNumLEDs() { return _numLEDs; }

uint8_t Controller::getBrightness() { return EEPROM.read(ADDR_BRIGHTNESS); }

uint8_t Controller::getEffect() { return EEPROM.read(ADDR_EFFECT); }

bool Controller::getEnabled() { return EEPROM.read(ADDR_ENABLED); }

CRGB Controller::getColor() { 
    return getColor(getCurColIdx() + _colOffset);
}

CRGB Controller::getColor(int idx) {
    CRGB color;
    if (idx < 0) idx = 0;
    if (idx > MAX_COLORS) idx = MAX_COLORS - 1;
    idx = ADDR_COLORS + idx * sizeof(CRGB);
    EEPROM.get<CRGB>(idx, color);
    return color;
}

int Controller::getOffset() {
    return _colOffset;
}

uint8_t Controller::getCurColIdx() { return EEPROM.read(ADDR_CURRENT_COLOR_IDX); }

uint8_t Controller::getFinColIdx() { return EEPROM.read(ADDR_FINAL_COLOR_IDX); }

#pragma endregion

#pragma region Interaction functions

void Controller::mainloop() 
{
    _commandHandler.ReadSerial();

    if (getEnabled()) {
        lFuncs[getEffect()](*this);
    } else {
        Effects::clear(*this);
    }
    FastLED.show();
}

void Controller::advanceColor() {
    _colOffset++;
    // Wrap around handling
    _colOffset = (_colOffset > getFinColIdx()) ? 0 : _colOffset;

    // Advance random color as well
    Effects::Random::randomise();
}

#pragma endregion

#pragma region SerialCommands command handlers

void commandFuncs::unrecognised(SerialCommands *sender, const char *cmd) 
{
    sender->GetSerial()->print("ERROR: '");
    sender->GetSerial()->print(cmd);
    sender->GetSerial()->println("' IS NOT RECOGNISED");
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
    Controller::getInstance()->setBrightness(newVal);
    sender->GetSerial()->println("OK");
}

void commandFuncs::editColor(SerialCommands *sender) 
{
    char *rInp = sender->Next();
    char *gInp = sender->Next();
    char *bInp = sender->Next();
    char *idxInp = sender->Next();

    // Get color at specified idx
    int idx = Controller::getInstance()->getCurColIdx();
    if (strlen(idxInp) != 0) {
        idx = atoi(idxInp);
        if (idx < 0 || idx >= MAX_COLORS) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(MAX_COLORS - 1);
            return;
        }
    }
    CRGB col = Controller::getInstance()->getColor(idx);

    // Check inputs are valid
    if (strlen(rInp) == 0 || strlen(gInp) == 0 || strlen(bInp) == 0) {
        sender->GetSerial()->println("ERROR: Must specify R, G, and B values");
        return;
    }

    col.r = atoi(rInp);
    col.g = atoi(gInp);
    col.b = atoi(bInp);

    Controller::getInstance()->setColor(col, idx);

    sender->GetSerial()->println("OK");
}

void commandFuncs::getColor(SerialCommands *sender) 
{
    char *idxInp = sender->Next();

    int idx = Controller::getInstance()->getCurColIdx();
    if (strlen(idxInp) != 0) {
        idx = atoi(idxInp);
        // Check if index is within range
        if (idx < 0 || idx >= MAX_COLORS) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(MAX_COLORS - 1);
            return;
        }
    }
    CRGB curCol = Controller::getInstance()->getColor(idx);

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
        sender->GetSerial()->println(Controller::getInstance()->getCurColIdx());
        return;
    }

    // Check given value in range
    if (newVal < 0 || newVal >= MAX_COLORS) {
        sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
        sender->GetSerial()->println(MAX_COLORS - 1);
        return;
    }

    // Update value
    Controller::getInstance()->setCurColIdx(newVal);
    // Reset current offset to prevent any out of range errors
    Controller::getInstance()->setOffset(0);
    sender->GetSerial()->println("OK");
}

void commandFuncs::effect(SerialCommands *sender) 
{   
    char *input = sender->Next();
    int newVal = atoi(input);

    // If new value is null report current value
    if (strlen(input) == 0) {
        sender->GetSerial()->println(Controller::getInstance()->getEffect());
        return;
    }

    // If new value is out of range, display error
    if (!(newVal <= arrayLength(lFuncs) - 1 && newVal >= 0)) {
        sender->GetSerial()->print("ERROR: Effect must be in range 0 - ");
        sender->GetSerial()->println(arrayLength(lFuncs) - 1);
        return;
    }

    Controller::getInstance()->setEffect(newVal);
    sender->GetSerial()->println("OK");
}

void commandFuncs::toggle(SerialCommands *sender)
{
    Controller::getInstance()->setEnabled(!Controller::getInstance()->getEnabled());
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
        sender->GetSerial()->println(Controller::getInstance()->getFinColIdx());
        return;
    }

    if (newVal < 0 || newVal >= MAX_COLORS) {
        sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
        sender->GetSerial()->println(MAX_COLORS - 1);
        return;
    }
    Controller::getInstance()->setFinColIdx(newVal);
    Controller::getInstance()->setOffset(0);
    sender->GetSerial()->println("OK");
}

#pragma endregion
