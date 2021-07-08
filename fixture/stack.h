

#ifndef STACK_H
#define STACK_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif


#include "externs.h"
#include "color.h"
#include "device.h"



void printFrames();


void printConeColorsFromMemory();

// generates a floating point number between 0 and 1, representing how far between x1 and x2, x is.   
// the assumption is that $x \in [x1, x2]$
float t(Time x, Time x1, Time x2);

// gives you a smoothly interpolated value between y1 and y2, given an $x \in [x1, x2]$, using the `t()` function.
float cubicNatural(Time x, Time x1, Time x2, uint8_t y1, uint8_t y2);




bool setNextFrameTime(Time duration);
bool setNextFrameColor(Cone cone, Color color);



// called once at the beginning of the program
// sets all frame_colors[] and frame_times[] values to BLACk and NO_EVENT_PLANNED.
void setupFrames();

// 
void renderFrame();


// for a specific `cone`,
// uses `frame_colors` and `frame_times` to figure out what the color should be,
// then sets the color to that.
bool transitionCone(Cone cone); // requires that the g_current_time has already been set externally.

// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllCones();

#endif // include guard
