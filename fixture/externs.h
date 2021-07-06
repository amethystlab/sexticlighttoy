#ifndef EXTERNS_H
#define EXTERNS_H

#include "defines.h"

extern Cone cycles[20]; // 20, because have 20 cones
extern uint8_t num_per_rotation;  // idk why 3.  is that just initial state?

// some global variables
extern Adafruit_NeoPixel pixels; // 140, because 20*7 = 140.
extern unsigned long previousMillis, currentMillis;

extern uint16_t pot1, pot2, pot3;

extern uint8_t buttons, switches;

extern unsigned long button_down_time; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
extern bool is_button_down; // is the button in the encoder being pushed?
extern int32_t rotary_counter; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)



////idk

extern uint8_t srcCone;
extern uint8_t nextCone;

//////////////rotation
extern Cone fiveFoldAxes[NUM_FIVE_FOLD_AXES][2];



extern ObjectMode mode;
extern int32_t previousEncoderValue; 
extern ObjectMode previousMode;

extern SymmetryType symmetry; 
extern uint16_t connections[20];


/////////////color

extern Color colorPresets[5]; 

////////////////stack

extern Color active_colors[20][2];
extern Time active_times[20][2];

extern Event event_stack[MAX_NUM_EVENTS];

extern Time start;
extern Time lengthOfShow;


extern Color experiment[NUM_LED_GROUPS][MAX_NUM_EVENTS];
#endif
