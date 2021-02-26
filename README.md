# LED Strip Controller
Controls an LED strip from an Arduino.\
Accepts serial input to control the lighting functions and colours used.\
Commands list shown [below](#commands)

## External libraries:
- FastLED - [here](https://www.arduino.cc/reference/en/libraries/fastled/)
- SerialCommands - [here](https://www.arduino.cc/reference/en/libraries/serialcommands/)

## Usage
Creating a Controller object:
```C++
// Serial rx and tx pins
const int rx = 11;
const int tx = 10;
Controller ledCon(rx, tx);
```

An array of LED's may then be provided after creating them with FastLED.\
An example is shown below:
```C++
#define LED_TYPE WS2812B
#define DATA_PIN 9
#define COLOR_ORDER GRB
#define NUM_LEDS 153

#define BLT_RX 11
#define BLT_TX 10

CRGB leds[NUM_LEDS];
Controller ledController(BLT_RX, BLT_TX);

void setup()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    ledController.setLEDs(leds, NUM_LEDS);
}

void loop()
{
    ledController.mainloop();
}
```

## Commands
The following commands can be sent over the specified serial channels to customise the behavior of the controller.

- `help`/`?` - Returns URL of this page, where list of commands is provided
- `toggle`/`t` - Toggle the strip on or off
- `bright <value(0-255)>` - Set the brightness of the strip
- `effect <value>` - Set current lighting effect
- `getcol <index>` - Get the colour at the given index, if none provided get the current colour
- `col <r(0-255)> <g(0-255)> <b(0-255)> <index>` - Set the colour at the given index, if none provided set current colour
- `setcol <index>` - Set the current active colour to the index specified
- `finalcol <index>` - Set final acitve color index

Color will be cycled between the value set with sc and fc.\
For example if color0 is red, color1 is green, and color2 is blue:\ 
sc would be set to 0, and fc would be set to 2. Colours would then cycle between these 3
