#include "StripController.h"

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
static void (*lFuncs[])(StripController&) = {
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

#pragma region Constructors

StripController::StripController(Stream *serial):
    _commandHandler(this, serial, _commandBuffer, sizeof _commandBuffer)
{
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

    // Initalise color index offset
    _colOffset = 0;

    // Load saved values
    FastLED.setBrightness(getBrightness());
    FastLED.setMaxRefreshRate(60);
}

StripController::StripController() : StripController(&Serial) {};

StripController::~StripController()
{
}

#pragma endregion

#pragma region Setters

void StripController::setLEDs(CRGB *leds, int numLEDs) { _leds = leds; _numLEDs = numLEDs; }

void StripController::setBrightness(uint8_t val) { EEPROM.update(ADDR_BRIGHTNESS, val); FastLED.setBrightness(val); }

void StripController::setEffect(uint8_t val) { EEPROM.update(ADDR_EFFECT, clamp(val, 0, arrayLength(lFuncs) - 1)); }

void StripController::setEnabled(bool val) { EEPROM.update(ADDR_ENABLED, val); }

void StripController::setColor(CRGB val) { setColor(val, getCurColIdx()); }

void StripController::setColor(CRGB val, int idx) {
    clamp(idx, 0, MAX_COLORS - 1);
    idx = ADDR_COLORS + idx * sizeof(CRGB);
    EEPROM.put<CRGB>(idx, val);
}

void StripController::setCurColIdx(uint8_t val) { 
    val = clamp(val, 0, MAX_COLORS);
    setFinColIdx(val + getFinColIdx());
    EEPROM.update(ADDR_CURRENT_COLOR_IDX, val);
}

void StripController::setFinColIdx(uint8_t val) {
    val = clamp(val, getCurColIdx(), MAX_COLORS - 1);
    EEPROM.update(ADDR_FINAL_COLOR_IDX, val); 
}

void StripController::setOffset(int val) {
    val = clamp(val, 0, getFinColIdx());
    _colOffset = val;
}

#pragma endregion

#pragma region Getters

CRGB* StripController::getLEDs() { return _leds; }

int StripController::getNumLEDs() { return _numLEDs; }

uint8_t StripController::getBrightness() { return EEPROM.read(ADDR_BRIGHTNESS); }

uint8_t StripController::getEffect() { return EEPROM.read(ADDR_EFFECT); }

bool StripController::getEnabled() { return EEPROM.read(ADDR_ENABLED); }

CRGB StripController::getColor() { 
    return getColor(getCurColIdx() + _colOffset);
}

CRGB StripController::getColor(int idx) {
    CRGB color;
    if (idx < 0) idx = 0;
    if (idx > MAX_COLORS) idx = MAX_COLORS - 1;
    idx = ADDR_COLORS + idx * sizeof(CRGB);
    EEPROM.get<CRGB>(idx, color);
    return color;
}

int StripController::getColOffset() {
    return _colOffset;
}

uint8_t StripController::getCurColIdx() { return EEPROM.read(ADDR_CURRENT_COLOR_IDX); }

uint8_t StripController::getFinColIdx() { return EEPROM.read(ADDR_FINAL_COLOR_IDX); }

#pragma endregion

#pragma region Interaction functions

void StripController::mainloop() 
{
    _commandHandler.ReadSerial();

    if (getEnabled()) {
        lFuncs[getEffect()](*this);
    } else {
        Effects::clear(*this);
    }
    FastLED.show();
}

void StripController::advanceColor() {
    _colOffset++;
    // Wrap around handling
    _colOffset = (_colOffset > getFinColIdx()) ? 0 : _colOffset;

    // Advance random color as well
    Effects::Random::randomise();
}

#pragma endregion

#pragma region SerialCommands command handlers

/**
 * Utility function used to get our controller instance from the given SerialCommands object.
 */
StripController* getController(SerialCommands *sender)  
{
    return ((ControllerSerialCommands*) sender)->getParent();
}

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
        if (idx < 0 || idx >= MAX_COLORS) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(MAX_COLORS - 1);
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
        if (idx < 0 || idx >= MAX_COLORS) {
            sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
            sender->GetSerial()->println(MAX_COLORS - 1);
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
    if (newVal < 0 || newVal >= MAX_COLORS) {
        sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
        sender->GetSerial()->println(MAX_COLORS - 1);
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
    if (!(newVal <= arrayLength(lFuncs) - 1 && newVal >= 0)) {
        sender->GetSerial()->print("ERROR: Effect must be in range 0 - ");
        sender->GetSerial()->println(arrayLength(lFuncs) - 1);
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

    if (newVal < 0 || newVal >= MAX_COLORS) {
        sender->GetSerial()->print("ERROR: Index must be in range 0 - ");
        sender->GetSerial()->println(MAX_COLORS - 1);
        return;
    }
    getController(sender)->setFinColIdx(newVal);
    getController(sender)->setOffset(0);
    sender->GetSerial()->println("OK");
}

#pragma endregion
