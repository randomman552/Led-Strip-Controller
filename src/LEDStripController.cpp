#include "LEDStripController.h"
#include "Effects/Effects.h"

namespace LEDStripController {
    int clamp(int val, int min, int max)
    {
        if (val > max) return max;
        else if (val < min) return min;
        return val;
    }

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
            setCurrentColorIndex(0);
            setFinalColorIndex(0);
            setFPS(60);
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
        FastLED.setMaxRefreshRate(getFPS());

        // Setup effects linked list
        effects = LinkedList<void (*)(Controller&)>();
        effects.add(Effects::Color::fill);
        effects.add(Effects::Color::alternateFill);
        effects.add(Effects::Color::fade);
        effects.add(Effects::Color::fillEmpty);
        effects.add(Effects::Color::fillEmptyMiddle);
        effects.add(Effects::Rainbow::fill);
        effects.add(Effects::Rainbow::fillEmpty);
        effects.add(Effects::Rainbow::cycle);
        effects.add(Effects::Rainbow::spinCycle);
        effects.add(Effects::Random::fill);
        effects.add(Effects::Random::fade);
        effects.add(Effects::Random::fillEmpty);
        effects.add(Effects::Random::fillEmptyMiddle);
    }

    Controller::~Controller()
    {
    }

    #pragma endregion

    #pragma region Setters

    void Controller::setLEDs(CRGB *leds, int numLEDs) { 
        _leds = leds;
        _numLEDs = numLEDs;
        // Ensure framerate is correct for new leds
        FastLED.setMaxRefreshRate(getFPS());
    }

    void Controller::setBrightness(uint8_t val) {
        EEPROM.update(Addrs::brightness, val); FastLED.setBrightness(val);
    }

    void Controller::setEffect(uint8_t val) {
        EEPROM.update(Addrs::effect, clamp(val, 0, effects.size() - 1));
    }

    void Controller::setEnabled(bool val) {
        EEPROM.update(Addrs::enabled, val);
    }

    void Controller::setColor(CRGB val) {
        setColor(val, getCurrentColorIndex());
    }

    void Controller::setColor(uint8_t r, uint8_t g, uint8_t b) {
        CRGB val(r, g, b);
        setColor(val);
    }

    void Controller::setColor(CRGB val, int idx) {
        idx = clamp(idx, 0, maxColors - 1);
        idx = Addrs::colors + idx * sizeof(CRGB);
        EEPROM.put<CRGB>(idx, val);
    }

    void Controller::setColor(uint8_t r, uint8_t g, uint8_t b, int idx) {
        CRGB val(r, g, b);
        setColor(val, idx);
    }

    void Controller::setFPS(uint8_t val) {
        val = clamp(val, 1, 255);
        EEPROM.update(Addrs::fps, val);
        FastLED.setMaxRefreshRate(val);
    }

    void Controller::setCurrentColorIndex(uint8_t val) { 
        val = clamp(val, 0, maxColors);
        setFinalColorIndex(val + getFinalColorIndex());
        EEPROM.update(Addrs::currentColorIdx, val);
    }

    void Controller::setFinalColorIndex(uint8_t val) {
        val = clamp(val, getCurrentColorIndex(), maxColors - 1);
        EEPROM.update(Addrs::finalColorIdx, val); 
    }

    void Controller::setColorOffset(int val) {
        val = clamp(val, 0, getFinalColorIndex());
        _colOffset = val;
    }

    #pragma endregion

    #pragma region Getters

    CRGB* Controller::getLEDs() {
        return _leds;
    }

    int Controller::getNumLEDs() {
        return _numLEDs;
    }

    uint8_t Controller::getBrightness() {
        return EEPROM.read(Addrs::brightness);
    }

    uint8_t Controller::getEffect() {
        return EEPROM.read(Addrs::effect);
    }

    bool Controller::getEnabled() {
        return EEPROM.read(Addrs::enabled);
    }

    CRGB Controller::getColor() { 
        return getColor(getCurrentColorIndex() + _colOffset);
    }

    CRGB Controller::getColor(int idx) {
        CRGB color;
        if (idx < 0) idx = 0;
        if (idx > maxColors) idx = maxColors - 1;
        idx = Addrs::colors + idx * sizeof(CRGB);
        EEPROM.get<CRGB>(idx, color);
        return color;
    }

    uint8_t Controller::getFPS() {
        return EEPROM.read(Addrs::fps);
    }

    int Controller::getColorOffset() {
        return _colOffset;
    }

    uint8_t Controller::getCurrentColorIndex() {
        return EEPROM.read(Addrs::currentColorIdx);
    }

    uint8_t Controller::getFinalColorIndex() {
        return EEPROM.read(Addrs::finalColorIdx);
    }

    #pragma endregion

    #pragma region Interaction functions

    void Controller::mainloop() 
    {
        if (getEnabled()) {
            effects[getEffect()](*this);
        } else {
            Effects::clear(*this);
        }
        FastLED.show();
    }

    void Controller::advanceColor() {
        _colOffset++;
        // Wrap around handling
        _colOffset = (_colOffset > getFinalColorIndex()) ? 0 : _colOffset;

        // Advance random color as well
        Effects::Random::randomise();
    }

    #pragma endregion
};
