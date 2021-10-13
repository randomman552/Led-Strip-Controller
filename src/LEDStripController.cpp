#include "LEDStripController.h"
#include "Effects/Effects.h"

#define arrayLength(array) sizeof(array) / sizeof(array[0])

namespace LEDStripController {
    #pragma region Constructors

    Controller::Controller()
    {
        // Check for EEPROM version mismatch
        if (EEPROM.read(Addrs::version) != version) {
            // Perform version update
            CRGB color(255, 255, 255);

            setEffect(0);
            setBrightness(64);
            setEnabled(true);
            setCurColIdx(0);
            setFinColIdx(0);
            EEPROM.update(Addrs::version, version);

            // Default all colors to white
            for (int i = 0; i < maxColors; i++)
            {
                setColor(color, i);
            }
        }
        // Initalise color index offset
        _colOffset = 0;

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

    void Controller::setBrightness(uint8_t val) { EEPROM.update(Addrs::brightness, val); FastLED.setBrightness(val); }

    void Controller::setEffect(uint8_t val) { EEPROM.update(Addrs::effect, clamp(val, 0, arrayLength(LEDStripController::lFuncs) - 1)); }

    void Controller::setEnabled(bool val) { EEPROM.update(Addrs::enabled, val); }

    void Controller::setColor(CRGB val) { setColor(val, getCurColIdx()); }

    void Controller::setColor(CRGB val, int idx) {
        clamp(idx, 0, maxColors - 1);
        idx = Addrs::colors + idx * sizeof(CRGB);
        EEPROM.put<CRGB>(idx, val);
    }

    void Controller::setCurColIdx(uint8_t val) { 
        val = clamp(val, 0, maxColors);
        setFinColIdx(val + getFinColIdx());
        EEPROM.update(Addrs::currentColorIdx, val);
    }

    void Controller::setFinColIdx(uint8_t val) {
        val = clamp(val, getCurColIdx(), maxColors - 1);
        EEPROM.update(Addrs::finalColorIdx, val); 
    }

    void Controller::setOffset(int val) {
        val = clamp(val, 0, getFinColIdx());
        _colOffset = val;
    }

    #pragma endregion

    #pragma region Getters

    CRGB* Controller::getLEDs() { return _leds; }

    int Controller::getNumLEDs() { return _numLEDs; }

    uint8_t Controller::getBrightness() { return EEPROM.read(Addrs::brightness); }

    uint8_t Controller::getEffect() { return EEPROM.read(Addrs::effect); }

    bool Controller::getEnabled() { return EEPROM.read(Addrs::enabled); }

    CRGB Controller::getColor() { 
        return getColor(getCurColIdx() + _colOffset);
    }

    CRGB Controller::getColor(int idx) {
        CRGB color;
        if (idx < 0) idx = 0;
        if (idx > maxColors) idx = maxColors - 1;
        idx = Addrs::colors + idx * sizeof(CRGB);
        EEPROM.get<CRGB>(idx, color);
        return color;
    }

    int Controller::getColOffset() {
        return _colOffset;
    }

    uint8_t Controller::getCurColIdx() { return EEPROM.read(Addrs::currentColorIdx); }

    uint8_t Controller::getFinColIdx() { return EEPROM.read(Addrs::finalColorIdx); }

    #pragma endregion

    #pragma region Interaction functions

    void Controller::mainloop() 
    {
        if (getEnabled()) {
            LEDStripController::lFuncs[getEffect()](*this);
        } else {
            LEDStripController::Effects::clear(*this);
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

    #pragma region Lighting functions array

    void (*lFuncs[])(Controller&) = {
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

    #pragma endregion
};
