#include "src/Effects.h"
#include <SoftwareSerial.h>
#include <SerialCommands.h>
#include <EEPROM.h>

// Define LED Setup Constants
#define DATA_PIN 9
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 153
#define MAX_FRAME_RATE 60


//Array to store leds in global scope (shows how much memory we have used during compilation)
CRGB leds[NUM_LEDS];


//Variable to store current effect index
int curEffect = 0;


//Variable to store whether led strip is currently to be enabled
bool enabled = true;

/**
 * An array of function pointers to the lighting functions.
 * This is used as a map between current effect integer and active lighting function.
 */
void(*lFuncs[])() {
    Effects::CustomColor::fill,
    Effects::CustomColor::fade,
    Effects::CustomColor::fillEmpty,
    Effects::CustomColor::fillEmptyMiddle,
    Effects::CustomColor::bounce,
    Effects::Rainbow::fill,
    Effects::Rainbow::fillEmpty,
    Effects::Rainbow::cycle,
    Effects::Rainbow::spinCycle
};


//Bluetooth serial setup
#define BLT_RX 11
#define BLT_TX 10
SoftwareSerial bltSerial(BLT_RX, BLT_TX);


//Serial commands setup
char commandBuffer[32];
SerialCommands commandHandler(&bltSerial, commandBuffer, sizeof commandBuffer);


#pragma region Memory mangement namespace

//Define memory addresses to store values in EEPROM. 
//You may wish to change these if you have written heavily to EEPROM on your arduino as it could result in inconsistent behaviour.
//Memory address of version number, if the value stored in EEPROM differs, memory will be wiped.
#define VERSION_ADDR 1
//Current version number
#define VERSION 1

#define EEPROM_EFFECT_ADDR 2
#define BRIGHTNESS_ADDR 3
#define ENABLED_ADDR 4
#define COLOR_ADDR 5


/**
 * Namespace containg memory related functions.
 * These functions are used to manage EEPROM for persistent storage when restarting.
 */
namespace Memory
{
    //Function used to reset the EEPROM to default values
    void reset() {
        //First, clear any values we currently have in EEPROM
        for (int i = 0; i < EEPROM.length(); i++)
            EEPROM.update(i, 255);

        CRGB defCol(255, 255, 255);

        //Then load in some default values
        EEPROM.update(EEPROM_EFFECT_ADDR, 0);
        EEPROM.update(BRIGHTNESS_ADDR, 64);
        EEPROM.update(ENABLED_ADDR, 1);
        EEPROM.put<CRGB>(COLOR_ADDR, defCol);

        //Set current version number
        EEPROM.update(VERSION_ADDR, VERSION);
    }

    //Loads the values from EEPROM into memory
    void load() {
        curEffect = EEPROM.read(EEPROM_EFFECT_ADDR);
        enabled = EEPROM.read(ENABLED_ADDR);
        FastLED.setBrightness(EEPROM.read(BRIGHTNESS_ADDR));
        EEPROM.get<CRGB>(COLOR_ADDR, Effects::color);
    }

    //Updates EEPROM with changed values
    void save() {
        EEPROM.update(EEPROM_EFFECT_ADDR, curEffect);
        EEPROM.update(BRIGHTNESS_ADDR, FastLED.getBrightness());
        EEPROM.update(ENABLED_ADDR, enabled);
        EEPROM.put<CRGB>(COLOR_ADDR, Effects::color);
    }

    //Function to initalise EEPROM for our use
    void setup() {
        // If version is not consistent, then we call reset
        if (EEPROM.read(VERSION_ADDR) != VERSION)
            reset();
        load();
    }
} // namespace Memory


//Undefine our memory addresses as they are no longer required.
#undef VERSION_ADDR
#undef VERSION
#undef BRIGHTNESS_ADDR
#undef ENABLED_ADDR
#undef COLOR_ADDR


#pragma endregion


#pragma region Commands namespace


/**
 * Namespace containing serial commands related functions and objects.
 */
namespace Commands
{
    /**
     * Namespace containing serial command functions
     */
    namespace funcs
    {
        /**
         * Unrecognised command handler
         * Tells the user that the command they have entered is not recognised
         */
        void unrecognised(SerialCommands *sender, const char *cmd) 
        {
            sender->GetSerial()->print("ERROR: '");
            sender->GetSerial()->print(cmd);
            sender->GetSerial()->println("' IS NOT RECOGNISED");
        }
        
        /**
         * Effect command, takes 1 argument (integer) specifying the effect to select
         */
        void effect(SerialCommands *sender)
        {
            char *input = sender->Next();
            int newVal = atoi(input);

            //If the new value is null, tell the user what the current value is
            if (input == NULL || !(newVal <= sizeof(lFuncs) / sizeof (lFuncs[0]) && newVal > 0)) {
                sender->GetSerial()->println(curEffect + 1);
                return;
            }
            curEffect = newVal - 1;
            sender->GetSerial()->println("OK");

            Memory::save();
        }

        //Toggle whether the led strip is enabled or disabled
        void toggle(SerialCommands *sender)
        {
            enabled = !enabled;
            sender->GetSerial()->println("OK");
            Memory::save();
        }

        //Change the current custom color
        void color(SerialCommands *sender)
        {
            char *rInp = sender->Next();
            char *gInp = sender->Next();
            char *bInp = sender->Next();

            if (rInp == NULL || gInp == NULL || bInp == NULL) {
                sender->GetSerial()->print(Effects::color.r);
                sender->GetSerial()->print(", ");
                sender->GetSerial()->print(Effects::color.g);
                sender->GetSerial()->print(", ");
                sender->GetSerial()->println(Effects::color.b);
                return;
            }

            CRGB newCol(atoi(rInp), atoi(gInp), atoi(bInp));
            Effects::color = newCol;

            sender->GetSerial()->println("OK");
            Memory::save();
        }

        //Change current brightness
        void brightness(SerialCommands *sender)
        {
            char *input = sender->Next();
            uint8_t newVal = atoi(input);

            //If no value was sent
            if (input == NULL | newVal > 255){
                sender->GetSerial()->println(FastLED.getBrightness());
                return;
            }
            FastLED.setBrightness(newVal);
            sender->GetSerial()->println("OK");
            Memory::save();
        }
    } // namespace Funcs

    /**
     * Namespace containing serial commands objects.
     */
    namespace cmds
    {
        SerialCommand effect("f", funcs::effect);
        SerialCommand color("c", funcs::color);
        SerialCommand brightness("b", funcs::brightness);
        SerialCommand toggle("t", funcs::toggle);
    } // namespace cmds

    //Function to be called on setup to set up the command handler
    void setup()
    {
        commandHandler.SetDefaultHandler(funcs::unrecognised);
        commandHandler.AddCommand(&cmds::effect);
        commandHandler.AddCommand(&cmds::toggle);
        commandHandler.AddCommand(&cmds::color);
        commandHandler.AddCommand(&cmds::brightness);
    }
} // namespace Commands


#pragma endregion


void setup() {
    Serial.begin(9600);
    Serial.println("Starting up...");

    bltSerial.begin(38400);

    //LED strip setup
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    Effects::numLEDs = NUM_LEDS;
    Effects::leds = leds;
    FastLED.setBrightness(64);
    FastLED.setMaxRefreshRate(MAX_FRAME_RATE);

    Commands::setup();

    Memory::setup();
    Memory::load();
    
    Serial.println("Setup Complete!");
}

void loop() {
    //Handle any required commands
    commandHandler.ReadSerial();

    //Display a frame of the current lighting effect (if currently enabled)
    if (enabled) {
        lFuncs[curEffect]();
    } else {
        Effects::clear();
    }
    FastLED.show();
}
