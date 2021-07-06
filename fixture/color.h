

#ifndef COLOR_H
#define COLOR_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif

#include "externs.h"


typedef struct {
    double red;       // a fraction between 0 and 1
    double green;       // a fraction between 0 and 1
    double blue;       // a fraction between 0 and 1
} RGB;


typedef struct {
    double hue;       // angle in degrees
    double saturation;       // a fraction between 0 and 1
    double value;       // a fraction between 0 and 1
} HSV;


void clear();
uint8_t random8BitValue();
void allConeRandom();
uint8_t restrictRandom8Bit();
void restrictAllConeRandom();

// gets the color of the pixels in a cone, assuming they're all pixels in the cone are the same color.  
Color getConeColor(Cone coneNum);

void pixelColor(int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w);
void pixToConeColor (int pixNum , uint8_t g, uint8_t r, uint8_t b, uint8_t w);

void coneColor (int coneNum, Color color);
void coneColor (int coneNum, uint8_t g, uint8_t r, uint8_t b, uint8_t w);

void setGroupPresetColor(uint8_t *coneArray, uint8_t degreeOfSymmetry);

void allred(unsigned long period);
void allblue(unsigned long period);
void allmagenta(unsigned long period);
void allgreen(unsigned long period);
void allwhite(unsigned long period);
void allLEDs(unsigned long period);


void doPulseMode();
#endif // include guard
