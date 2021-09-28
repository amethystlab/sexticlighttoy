

#ifndef PULSE_H
#define PULSE_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif

#include "color.h"
#include "device.h"




void pulse_pot_color(unsigned long time_per_step);
void pulse_fixed_color(unsigned long time_per_step, IndividualColor red, IndividualColor green, IndividualColor blue, IndividualColor white);

void pulse_red(unsigned long time_per_step);
void pulse_blue(unsigned long time_per_step);
void pulse_magenta(unsigned long time_per_step);
void pulse_green(unsigned long time_per_step);
void pulse_white(unsigned long time_per_step);
void pulse_all(unsigned long time_per_step);


void doPulseMode();



#endif // include guards

