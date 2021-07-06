

#ifndef STACK_H
#define STACK_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif


#include "externs.h"
#include "color.h"



void printStack();
void printEvent(Event e);
void printActive();
void printConeColorsFromMemory();

// generates a floating point number between 0 and 1, representing how far between x1 and x2, x is.   
// the assumption is that $x \in [x1, x2]$
float t(Time x, Time x1, Time x2);

// gives you a smoothly interpolated value between y1 and y2, given an $x \in [x1, x2]$, using the `t()` function.
float cubicNatural(Time x, Time x1, Time x2, uint8_t y1, uint8_t y2);





// will make `cone` transition from its previous color to the specified color, taking the specified duration.
bool addEventToStack(Cone cone, Color color, Time duration);


// loops over all cones.
// if the active_time indicates that NO_EVENT_PLANNED,
// then the old active time becomes now, and the old active color is the current cone color.
// 
// then, loop over all events on the stack, and if one refers to the current cone,
// 
void eventStackToActive();



// called once at the beginning of the program
// sets all event_cone[] and times[] values to OPEN_EVENT_CODE and NO_EVENT_PLANNED.
void setupEvents();

// 
void doEventMode();


// for a specific `cone`,
// uses `colors` and `times` to figure out what the color should be,
// then sets the color to that.
bool transitionCone(Cone cone); // requires that the current_time has already been set externally.

// loops over all cones in the fixture, and calls `transitionCone` for each.
void transitionAllCones();

#endif // include guard
