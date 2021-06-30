

#ifndef COLOR_H
#define COLOR_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif

#include "externs.h"








void clear();
uint8_t random8BitValue();
void allConeRandom();
uint8_t restrictRandom8Bit();
void restrictAllConeRandom();


void pixelColor(int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w);
void pixToConeColor (int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w);
void coneColor (int coneNum, uint8_t g, uint8_t r, uint8_t b, uint8_t w);

void setGroupPresetColor(uint8_t *coneArray, uint8_t degreeOfSymmetry);

void allred(unsigned long period);
void allblue(unsigned long period);
void allmagenta(unsigned long period);
void allgreen(unsigned long period);
void allwhite(unsigned long period);
void allLEDs(unsigned long period);
#endif // include guard
