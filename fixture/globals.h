
#ifndef GLOBALS_H
#define GLOBALS_H

#include "defines.h"




/*
 * Array to store the cyclic groups of rotation and reflection
*/
uint8_t cycles[NUM_LED_GROUPS]; 

uint8_t num_per_rotation = 3;  // idk why 3.  is that just initial state?

// some global variables
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LED_GROUPS*NUM_PIXELS_PER_GROUP, NEOPIXEL_PIN,  NEO_RGBW + NEO_KHZ800); // 140, because 20*7 = 140.
unsigned long previousMillis = 0, currentMillis = 0;

uint16_t pot1, pot2, pot3;

uint8_t buttons, switches; // can take up to 8 of each.  we'll play bit games with them.

unsigned long button_down_time = 0L; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
bool button_down = 0; // is the button in the encoder being pushed?
int32_t rotary_counter = 0; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)




ObjectMode mode = Event;
SymmetryType symmetry = TwoFold; 

int32_t previousEncoderValue = 0; 
ObjectMode previousMode;
/*---------------------------------------------------------------------*/


// define the connections between cones.
// Maybe it's one of these?  Cross those fingers!  Explanation below.
// 
/*
 * Each uint16_t stores 3 numbers.
 * The index represents a specific cone on the barth sextic,
 * whereas the values stored inside the uint16_t hold the cone numbers
 * that are connected
 * 
 * ________________________________________________________________________________
 * | First Connected Cone | Second Connected Cone | Third Connected Cone  |        |
 * |-------------------------------------------------------------------------------|
 * | 5 Bits               | 5 Bits                | 5 Bits                | 1 Bit  |
 * |-------------------------------------------------------------------------------|
 * 
 * The connections are ordered such that if you were to hold the geometric object
 * from the cone specified by the index, the ordering of the connections would follow
 * a circle around the cone, ANTI-clockwise from above, looking from the point of the cone
 * to the center of the fixture.
 *
 * Math question (combinatorics): How many valid numberings are there?
 * 
 * TODO: INDICATE ONE OFF INDEXING HANDLED BY GET_CONNECTION FUNCTION
 *  
 * This is packed into uint16_ts to save space, as the arduino platform is limited in memory.
 * 
 */
// we're specifying which cones are connected to each other.  order matters.
// Order *must* be the same for all, as you look down the cones.
#if FIXTURE==1 
uint16_t connections[NUM_LED_GROUPS] = { // Will's.  The first one silviana made.
  MAKE_CONNECTION(4,1,7), // specifying that cone 0 is connected to cones 4, 1, and 7, in that order.  
  MAKE_CONNECTION(9,0,2),
  MAKE_CONNECTION(11,1,3),
  MAKE_CONNECTION(4,13,2),
  MAKE_CONNECTION(3,0,5),

  MAKE_CONNECTION(6,14,4),
  MAKE_CONNECTION(5,7,19),
  MAKE_CONNECTION(8,6,0),
  MAKE_CONNECTION(18,7,9),
  MAKE_CONNECTION(8,1,10),

  MAKE_CONNECTION(17,9,11),
  MAKE_CONNECTION(12,10,2),
  MAKE_CONNECTION(16,11,13),
  MAKE_CONNECTION(14,12,3),
  MAKE_CONNECTION(15,13,5),

  MAKE_CONNECTION(16,14,19),
  MAKE_CONNECTION(12,15,17),
  MAKE_CONNECTION(10,16,18),
  MAKE_CONNECTION(17,19,8),
  MAKE_CONNECTION(18,15,6)
};

#elif FIXTURE==2 
uint16_t connections[NUM_LED_GROUPS] = { // Samantha's.  The second one.  Sadly not identical.
  MAKE_CONNECTION(4,1,13),
  MAKE_CONNECTION(0,2,11),
  MAKE_CONNECTION(9,1,3),
  MAKE_CONNECTION(7,2,4),
  MAKE_CONNECTION(0,5,3),

  MAKE_CONNECTION(14,6,4),
  MAKE_CONNECTION(16,7,5),
  MAKE_CONNECTION(8,3,6),
  MAKE_CONNECTION(17,9,7),
  MAKE_CONNECTION(10,2,8),

  MAKE_CONNECTION(18,11,9),
  MAKE_CONNECTION(12,1,10),
  MAKE_CONNECTION(19,13,11),
  MAKE_CONNECTION(14,0,12),
  MAKE_CONNECTION(15,5,13),

  MAKE_CONNECTION(19,16,14),
  MAKE_CONNECTION(17,6,15),
  MAKE_CONNECTION(18,8,16),
  MAKE_CONNECTION(19,10,17),
  MAKE_CONNECTION(15,12,18)
};
#endif

///////idk

uint8_t srcCone = 1;
uint8_t nextCone = 8;



////////////////rotation
uint8_t fiveFoldAxes[NUM_FIVE_FOLD_AXES][2] = {{1, 5}, {6, 15}, {14, 13}, {12, 11}, {1, 6}, {8, 7}, {15, 16}, {13, 17}, {11, 18}, {9, 19}, {7, 20}, {18, 17}};



/////////////color
//use an array of 5 since at most we are looking at 5-fold rotational symmetry (g,r,b,w,y)
uint32_t colorPresets[5] = {GREEN, RED, BLUE, WHITE, YELLOW}; 


/////////////stack

uint32_t colors[NUM_LED_GROUPS][2];
uint16_t times[NUM_LED_GROUPS][2];

uint8_t event_cone[MAX_NUM_EVENTS];
uint32_t event_colors[MAX_NUM_EVENTS];
uint16_t event_times[MAX_NUM_EVENTS];

unsigned long start;
uint16_t lengthOfShow;

#endif // include guard
