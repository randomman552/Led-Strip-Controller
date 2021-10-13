# LED Strip Controller
Library to control an LED strip from an Arduino.\
All elements of this library are held under the LEDStripController namespace.

## Dependencies
- FastLED - [here](https://www.arduino.cc/reference/en/libraries/fastled/)
- SerialCommands - [here](https://www.arduino.cc/reference/en/libraries/serialcommands/)

## Classes
- Controller
    - Contains various getters and setters to control which lighting functions and colors are used
- SerialController
    - Sub-class of Controller
    - Inherits all of the same methods to control lighting functions
    - Intercepts commands from a given stream and changes it's own state accordingly.
    - When constructing an object of this class, an object of the Stream class can be provided.
        - This object is what commands will be recieved through.
    - If no stream is provided, Serial will be used by default.
    - Commands for this class are shown [here](#commands)

## Usage
To create a normal Controller object (with no serial attachment for commands), do the following:
```C++
Controller ledCon;
```
Note: This Controller will not yet do anything, in the setup function you will need to:
- Create an array of leds using FastLED.
- Call the Controller.setLEDs method to provide this array of leds to the Controller.
- Add a Controller.mainloop call to the loop function (this is what applies the lighting to the leds).

An example of a program using this library is shown below.\
I use this for my own bluetooth controlled led strip.
```C++
#define LED_TYPE WS2812B
#define DATA_PIN 9
#define COLOR_ORDER GRB
#define NUM_LEDS 60

#define BLT_RX 11
#define BLT_TX 10

CRGB leds[NUM_LEDS];
SoftwareSerial bltSerial(BLT_RX, BLT_TX);
Controller ledController(&bltSerial);

void setup()
{
    bltSerial.begin(38400);
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    ledController.setLEDs(leds, NUM_LEDS);
}

void loop()
{
    ledController.mainloop();
}

```

## Provided lighting functions
There are 13 lighting functions currently supplied with this library.
### User defined color functions
- Use the colors defined by the user
- Will cycle between user defined colors
0. Single color fill - Fill the strip with a solid color
1. Alternate fill - Fill the strip with alternating colors based on the colors stored in memory
2. Single color fade - Fade the given color in and out
3. Single color wipe - Fills a single color from one end of the strip to the other
4. Single color wipe (center) - A variant of the previous function that fills from the center

### Rainbow color functions
5. Rainbow fill - Fill the strip with a rainbow gradient
6. Rainbow wipe - Same as wipe function but with rainbow gradient
7. Rainbow shift - Shift the hue of the led strip constantly, creating a rainbow effect
8. Rainbow cycle - Cycle a rainbow gradient down the strip continuously

### Random color functions
- These functions generate a random color, this color is changed after a certain number of frames
9. Random color fill - Fill's the strip with a random solid color
10. Random color fade - Fade a random color in and out
11. Random color wipe - Same as color wipe, but with a random color
12. Random color wipe (center) - Same as above

## Commands
The following commands can be sent over the provided stream to alter the behaviour of SerialController.

- `help`/`?` - Returns URL of this page, where list of commands is provided
- `toggle`/`t` - Toggle the strip on or off
- `bright <value(0-255)>` - Set the brightness of the strip
- `effect <value>` - Set current lighting effect
- `getcol <index>` - Get the colour at the given index, if none provided get the current colour
- `col <r(0-255)> <g(0-255)> <b(0-255)> <index>` - Set the colour at the given index, if none provided set current colour
- `curcol <index>` - Set the current active colour to the index specified
- `finalcol <index>` - Set final acitve color index

Color will be cycled between the value set with `curcol` and `finalcol`.\
For example if color0 is red, color1 is green, and color2 is blue:\ 
sc would be set to 0, and fc would be set to 2. Colours would then cycle between red, green, and blue.
