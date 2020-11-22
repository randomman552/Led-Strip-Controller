#include "src/Effects.h"
#include <SoftwareSerial.h>
#include <SerialCommands.h>

// Define LED Setup Constants
#define DATA_PIN 9
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 144
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


#pragma region Serial commands namespace and definitions


namespace Commands
{
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
            return;
        }

        //Toggle whether the led strip is enabled or disabled
        void toggle(SerialCommands *sender)
        {
            enabled = !enabled;
            sender->GetSerial()->println("OK");
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
        }
    } // namespace Funcs

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
