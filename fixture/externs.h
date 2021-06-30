#ifndef EXTERNS_H
#define EXTERNS_H

#include "defines.h"

extern uint8_t cycles[20]; // 20, because have 20 cones
extern uint8_t num_per_rotation;  // idk why 3.  is that just initial state?

// some global variables
extern Adafruit_NeoPixel pixels; // 140, because 20*7 = 140.
extern unsigned long previousMillis, currentMillis;

extern uint16_t pot1, pot2, pot3;

extern bool buttons[3], switches[3];

extern unsigned long button_down_time; // [ms] the time the encoder button has been pushed so far.  is 0 if the button's not being pressed
extern bool button_down; // is the button in the encoder being pushed?
extern int32_t rotary_counter; // current "position" of rotary encoder (increments clockwise, decrements anticlockwise)



////idk

extern uint8_t srcCone;
extern uint8_t nextCone;

//////////////rotation
extern uint8_t fiveFoldAxes[NUM_FIVE_FOLD_AXES][2];



extern ObjectMode mode;
extern int32_t previousEncoderValue; 
extern ObjectMode previousMode;

extern SymmetryType symmetry; 
extern uint16_t connections[20];


/////////////color

extern uint32_t colorPresets[5]; 

////////////////stack

extern uint32_t colors[20][2];
extern uint16_t times[20][2];

extern uint8_t event_cone[MAX_NUM_EVENTS];
extern uint32_t event_colors[MAX_NUM_EVENTS];
extern uint16_t event_times[MAX_NUM_EVENTS];

extern unsigned long start;
extern uint16_t lengthOfShow;

#endif
