

#ifndef STACK_H
#define STACK_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif


#include "externs.h"
#include "color.h"


uint32_t getConeColor(uint8_t coneNum);
bool addEventToStack(uint8_t cone, uint32_t color, uint16_t timeOfEvent);
void eventStackToTransition();

float t(uint16_t x, uint16_t x1, uint16_t x2);
float cubicNatural(uint16_t x, uint16_t x1, uint16_t x2, uint8_t y1, uint8_t y2);
bool transitionCone(uint8_t coneNum, bool repeat);

#endif // include guard
