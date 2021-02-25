#include "./Controller.h"

#define ADDR_VERSION 1
#define ADDR_EFFECT 2
#define ADDR_BRIGHTNESS 3
#define ADDR_ENABLED 4
#define ADDR_COLOR 5

#define VERSION 1


/**
 * Array of lighting function pointers
 * Allows us to call an lighting function by index rather than by name
 */
static void (*lFuncs[])(Controller&) = {
    Effects::Color::fill,
    Effects::Color::fade,
    Effects::Color::fillEmpty,
    Effects::Color::fillEmptyMiddle,
    Effects::Rainbow::fill,
    Effects::Rainbow::fillEmpty,
    Effects::Rainbow::cycle,
    Effects::Rainbow::spinCycle
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
        CRGB color(255, 255, 255);

        EEPROM.update(ADDR_EFFECT, 0);
        EEPROM.update(ADDR_BRIGHTNESS, 64);
        EEPROM.update(ADDR_ENABLED, true);
        EEPROM.put<CRGB>(ADDR_COLOR, color);
    }

    // Setup command handler
    _commandHandler.SetDefaultHandler(commandFuncs::unrecognised);
    _commandHandler.AddCommand(new SerialCommand("f", commandFuncs::effect));
    _commandHandler.AddCommand(new SerialCommand("c", commandFuncs::color));
    _commandHandler.AddCommand(new SerialCommand("b", commandFuncs::brightness));
    _commandHandler.AddCommand(new SerialCommand("t", commandFuncs::toggle));

    // Set singleton instance
    _instance = this;

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
void Controller::setEffect(uint8_t val) { EEPROM.update(ADDR_EFFECT, val); }
void Controller::setEnabled(bool val) { EEPROM.update(ADDR_ENABLED, val); }
void Controller::setColor(CRGB val) { EEPROM.put<CRGB>(ADDR_COLOR, val); }

#pragma endregion

#pragma region Getters

CRGB* Controller::getLEDs() { return _leds; }
int Controller::getNumLEDs() { return _numLEDs; }
uint8_t Controller::getBrightness() { return EEPROM.read(ADDR_BRIGHTNESS); }
uint8_t Controller::getEffect() { return EEPROM.read(ADDR_EFFECT); }
bool Controller::getEnabled() { return EEPROM.read(ADDR_ENABLED); }
CRGB Controller::getColor() { CRGB color; return EEPROM.get<CRGB>(ADDR_COLOR, color); }

#pragma endregion

#pragma region Frame drawing

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

void commandFuncs::color(SerialCommands *sender) 
{
    char *rInp = sender->Next();
    char *gInp = sender->Next();
    char *bInp = sender->Next();
    CRGB curCol = Controller::getInstance()->getColor();

    // Check for no input
    if (strlen(rInp) == 0 || strlen(gInp) == 0 || strlen(bInp) == 0) {
        sender->GetSerial()->print(curCol.r);
        sender->GetSerial()->print(", ");
        sender->GetSerial()->print(curCol.g);
        sender->GetSerial()->print(", ");
        sender->GetSerial()->println(curCol.b);
        return;
    }

    CRGB newCol(atoi(rInp), atoi(gInp), atoi(bInp));
    Controller::getInstance()->setColor(newCol);

    sender->GetSerial()->println("OK");
}

void commandFuncs::effect(SerialCommands *sender) 
{   
    char *input = sender->Next();
    int newVal = atoi(input);

    // If new value is null or out of bounds, report current value
    if (strlen(input) == 0 || !(newVal <= sizeof(lFuncs) / sizeof (lFuncs[0]) && newVal >= 0)){
        sender->GetSerial()->println(Controller::getInstance()->getEffect());
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

#pragma endregion
