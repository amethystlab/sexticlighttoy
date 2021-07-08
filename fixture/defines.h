
#ifndef DEFINES_H
#define DEFINES_H

//#define DEBUG_PRINT // uncomment to make tons of things print. 
#define WILLIAM_DEBUG
#define TEST(actual, expected) Serial.print("expected: "); Serial.print(expected); Serial.print("actual: "); Serial.println(actual)


#include <Adafruit_NeoPixel.h> // see https://github.com/adafruit/Adafruit_NeoPixel

#define FIXTURE_ADDRESS 4  // used with i2c.  must be the same as the value in controller.ino.
#define NEOPIXEL_PIN 12 // The pin you used on your Arduino when wiring the neopixels. 

#define NUM_BUTTONS 3
#define NUM_SWITCHES 3

// MACROS TO EASE WITH THE PROCESS OF WRITING THIS OUT AS UINT16_T
#define MAX_UINT5 31 // 2^5-1

#define CONNECTION_NUM_SETUP(num_connection, num_cone) ((num_cone << 5*(num_connection)) & (MAX_UINT5 << 5*(num_connection)))
#define MAKE_CONNECTION(a,b,c) CONNECTION_NUM_SETUP(0, a) | CONNECTION_NUM_SETUP(1, b) | CONNECTION_NUM_SETUP(2, c)

#define MAX_CONNECTION_NUM 3
 
#define NUM_LED_GROUPS 20 // 20 cones total on fixture
#define NUM_CONES NUM_LED_GROUPS
#define NUM_PIXELS_PER_GROUP 7 // I used 7-pixel chips in the sextic toys

#define NUM_PIXELS NUM_LED_GROUPS*NUM_PIXELS_PER_GROUP

#define FIXTURE 1  // select which physical wiring path you implemented.  see globals.h  

// a list of the symmetries of the icosahedron.  stored in a global.  see globals.h
typedef enum SymmetryType {
  TwoFold,
  ThreeFold,
  FiveFold,
  Reflect
};

// which mode is the light fixture in?  stored in a global.  see globals.h
typedef enum ObjectMode {
  ColorSet=0,
  Rotational=1,
  Reflectional=2,
  EventMode=5,
  Pulse=6,
  Diagnostic=7
};

using Cone = uint8_t;
using Direction = int8_t;
using Color = uint32_t;
using IndividualColor = uint8_t;
using Hue = uint16_t;
using Time = unsigned long;

/////////////////////rotation



#define POSITIVE 1
#define NEGATIVE -1

#define CONE_DNE 211 // Error code returned by get_connection when provided a nonexistant cone number 
#define CONNECTION_DNE 212 // Error code returned by get_connection when provided a connection number that is too large
#define SRC_NEXT_UNCONNECTED_ERR 213

#define NO_SUCH_CONE_ERR 213
#define NO_CONNECTION 215
#define NO_SUCH_DIRECTION 216

#define NUM_FIVE_FOLD_AXES 12
#define NUM_TICKS 3

#define TICKS_PER_ROTATION 3 // how many ticks on the encoder it takes to trigger a rotation.



/////////////color
// define some hexadecimal colors.  Handy!
#define WHITE 0xFFFFFF

#define RED 0x00FF00
#define GREEN 0xFF0000
#define BLUE 0x0000FF

#define YELLOW 0xFFFF00
#define MAGENTA 0xFF00FF
#define CYAN 0x00FFFF

#define BLACK 0x000000

#define sumFromZeroToNineteen 190 // used in set_missing_in_cycles





////////////stack



#define GRADUAL_TRANSITION
// #define CUBIC_INTERP // if this is not defined, we'll use linear interpolation.

#define NO_EVENT_PLANNED 0 // i think this should be 0 or the max for unsigned long (Time)
#define MAX_NUM_EVENTS 22 // makes things not render if 23 or higher.
#define OPEN_EVENT_CODE (NUM_CONES+1)


#endif
