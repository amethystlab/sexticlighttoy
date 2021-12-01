

#ifndef PULSE_H
#define PULSE_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif

#include "color.h"
#include "device.h"




void pulse_pot_color(Time time_per_step);
void pulse_fixed_color(Time time_per_step, IndividualColor red, IndividualColor green, IndividualColor blue, IndividualColor white);

void pulse_red(Time time_per_step);
void pulse_blue(Time time_per_step);
void pulse_magenta(Time time_per_step);
void pulse_green(Time time_per_step);
void pulse_white(Time time_per_step);
void pulse_all(Time time_per_step);


void doPulseMode();



#endif // include guards

