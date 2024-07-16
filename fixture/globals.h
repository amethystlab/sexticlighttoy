
#ifndef GLOBALS_H
#define GLOBALS_H

#include "defines.h"




Cone cycles[NUM_LED_GROUPS]; //< Array to store the cyclic groups of rotation and reflection



Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN,  NEO_RGBW + NEO_KHZ800); // 140, because 20*7 = 140.
Time g_previous_millis = 0, g_current_millis = 0;

uint16_t pot0, pot1, pot2;

uint8_t buttons, switches; // can take up to 8 of each.  we'll play bit games with them.

Time button_down_time = 0L; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
bool is_encoder_button_down = 0; // is the button in the encoder being pushed?

int32_t encoder_counter = 0; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)
int32_t previous_encoder_value = 0; 



ObjectMode g_mode = AutoRotate;
ObjectMode previousMode;

uint16_t g_auto_color_rotation = 0;

SymmetryType g_symmetry = TwoFold; 
SymmetryType g_previous_symmetry = FiveFold;



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
 * This is packed into uint16_ts to save space, as the arduino platform is limited in memory.
 * 
 * Math question (combinatorics): How many valid numberings are there?
 */
// we're specifying which cones are connected to each other.  order matters.
// Order *must* be the same for all, as you look down the cones.  
#if FIXTURE==1 
 uint16_t connections[NUM_LED_GROUPS] = { // Fixture 1.  The first one silviana made, and Will used for programming Summer 2020
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

// these hardcoded values are incorrect unless Fixture == 1
// please write documentation on how these were generated
// so that if someone makes another one, they can 
// figure these out, too.
// does order of the pair matter?
Cone fiveFoldAxes[NUM_FIVE_FOLD_AXES][2] = {{1, 5}, {6, 15}, {14, 13}, {12, 11}, {1, 6}, {8, 7}, {15, 16}, {13, 17}, {11, 18}, {9, 19}, {7, 20}, {18, 17}};

#elif FIXTURE==2 || FIXTURE==3
// fixture #2, Samantha's.  Sadly not identical to Fixture 1.  
// also, the clear TPU one silviana made Fall 2021 (Fixture 3)

 uint16_t connections[NUM_LED_GROUPS] = { 
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

Cone fiveFoldAxes[NUM_FIVE_FOLD_AXES][2] = {{1, 5}, {6, 15}, {14, 13}, {12, 11}, {1, 6}, {8, 7}, {15, 16}, {13, 17}, {11, 18}, {9, 19}, {7, 20}, {18, 17}};
// ^^^ those are wrong.  but i also think they're unnecessary.

#endif

///////idk

Cone current_cone[3]{0,1,2};




////////////////rotation

float pi = acos(-1);
float phi = (sqrt(5)+1)/2;

  // these magic constants computed with the help of python.
  // i wrote a notebook to compute them, based on 
  // rotating the points on a dodecahedron
  // and projecting them to the xy plane,
  // then using atan2, my favorite lil function.
  // this array should be computed in memory, instead of every time this
  // function hits.  when you next read this, please 
  // make this modification.  

float twofold_distances[NUM_CONES] = {phi-1, phi-1,
                            sqrt(2),sqrt(2),sqrt(2),sqrt(2),
                            phi,phi,
                            sqrt(3),sqrt(3),sqrt(3),sqrt(3),
                            phi,phi,
                            sqrt(2),sqrt(2),sqrt(2),sqrt(2),
                            phi-1, phi-1
                          };

float twofold_angles[NUM_CONES] = {0,pi,   
                        3*pi/4 , 5*pi/4      ,  7*pi/4   , pi/4,
                        pi/2, 3*pi/2,     
                        -2.7767288254763103+2*pi,2.7767288254763103,0.3648638281134831,-0.3648638281134831+2*pi, // ,   
                        pi/2, 3*pi/2,
                        pi/4, 3*pi/4, 5*pi/4, 7*pi/4, 
                        pi,0
                      };


float g_a3 = 0.6590580358263686; // 37.76 deg, green-yellow
float g_b3 = 1.435337066566721;  // 82.24 deg, orange probably, to yellow
float g_c3 = 0.3881395153702137; // 22.24 deg, green, to slightly yellow
float g_d3 = 1.7062555870230722; // 97.76 deg, reddish

float g_e3 = pi/3;


float g_s2_3 = 2*pi/3;
float g_s4_3 = 4*pi/3;

// correct if the root node is 17. also appears correct for all roots.

float threefold_angles[NUM_CONES] = {0, // ill-defined angle, so 0

                               0, g_s2_3, g_s4_3,    // easily seen equilateral triangle directly adjacent to root cone

                               g_b3, g_b3+g_s2_3, g_b3+g_s4_3,  
                               g_a3, g_a3+g_s2_3, g_a3+g_s4_3,   // two slightly shifted triangles, distance 2 from root cone
                               
                               
                               g_c3+g_s2_3, g_c3+g_s4_3, g_c3, // analagous structures on bottom side (root is top)
                               g_d3,g_d3+g_s2_3, g_d3+g_s4_3, 

                               g_e3+g_s2_3, g_e3+g_s4_3, g_e3, // easily seen equilateral triangle directly adjacent to root cone

                               0 // ill-defined angle, so 0
                             };


float threefold_distances[NUM_CONES] = {0,
                                  1.1547005383791311,1.1547005383791311,1.1547005383791311,
                                  
                                  1.6329931618555373,1.6329931618555373,1.6329931618555373,
                                  1.6329931618555373,1.6329931618555373,1.6329931618555373,

                                  1.6329931618555373,1.6329931618555373,1.6329931618555373,
                                  1.6329931618555373,1.6329931618555373,1.6329931618555373,

                                  1.1547005383791311,1.1547005383791311,1.1547005383791311,

                                  0
                                };




float g_a5 = 1.2566370614359172; // 72 degrees
float g_b5 = g_a5/2; // 36 degrees -- the pentagons on the bottom half are rotated to bisect the angle

float fivefold_angles[NUM_CONES] = {0,g_a5,g_a5+g_a5,3*g_a5, 4*g_a5, // these are the same, amazingly enough
                               0,g_a5,g_a5+g_a5,3*g_a5, 4*g_a5,
                               g_b5,g_a5+g_b5,2*g_a5+g_b5, 3*g_a5+g_b5, 4*g_a5+g_b5,
                               g_b5,g_a5+g_b5,2*g_a5+g_b5, 3*g_a5+g_b5, 4*g_a5+g_b5
                              };



/////////////color

//use an array of 5 since at most we are looking at 5-fold rotational symmetry (g,r,b,w,y)
Color colorPresets[5] = {GREEN, RED, BLUE, WHITE, YELLOW}; 


/////////////stack

// the currently active state,
// between which we are interpolating.
// the `active_times` define the start and end time for the current event,
// and the `active_colors` store the before and after color, between which we interpolate.
Color frame_colors[2][NUM_LED_GROUPS];
Time frame_times[2];

Color most_recent_colors[NUM_LED_GROUPS];

Time g_current_time;



#endif // include guard
