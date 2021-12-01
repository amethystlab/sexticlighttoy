#ifndef EXTERNS_H
#define EXTERNS_H

#include "defines.h"

extern Cone cycles[20]; // 20, because have 20 cones
extern uint8_t num_per_rotation;  // idk why 3.  is that just initial state?

// some global variables
extern Adafruit_NeoPixel pixels; // 140, because 20*7 = 140.
extern unsigned long g_previous_millis, g_current_millis;

extern uint16_t pot0, pot1, pot2;

extern uint8_t buttons, switches;

extern unsigned long button_down_time; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
extern bool is_button_down; // is the button in the encoder being pushed?
extern int32_t rotary_counter; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)
extern int32_t prev_rotary_counter;







//////////////rotation

extern Cone current_cone[3];

extern Cone fiveFoldAxes[NUM_FIVE_FOLD_AXES][2];

extern float pi;
extern float phi;

extern float twofold_distances[NUM_CONES];
extern float twofold_angles[NUM_CONES];

extern float threefold_distances[NUM_CONES];
extern float threefold_angles[NUM_CONES];

extern float fivefold_distances[NUM_CONES];
extern float fivefold_angles[NUM_CONES];



extern ObjectMode g_mode;
extern int32_t previousEncoderValue; 
extern ObjectMode previousMode;

extern SymmetryType g_symmetry; 
extern SymmetryType g_previous_symmetry; 
extern uint16_t connections[NUM_LED_GROUPS];


/////////////color

extern Color colorPresets[5]; 

//////////////// frame butter stuff

extern Color frame_colors[2][NUM_LED_GROUPS];
extern Time frame_times[2];

extern Color most_recent_colors[NUM_LED_GROUPS];

extern Time g_current_time;


#endif
