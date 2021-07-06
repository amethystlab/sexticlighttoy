

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
Color getConeColor(Cone cone_num);

void pixelColor(int pix_num , IndividualColor g, IndividualColor r, IndividualColor b, IndividualColor w);
void pixToConeColor (int pix_num , IndividualColor g, IndividualColor r, IndividualColor b, IndividualColor w);

void coneColor(Cone cone_num, Color color);
void coneColor(Cone cone_num, IndividualColor g, IndividualColor r, IndividualColor b, IndividualColor w);

void setGroupPresetColor(uint8_t *coneArray, uint8_t degreeOfSymmetry);

void allred(unsigned long period);
void allblue(unsigned long period);
void allmagenta(unsigned long period);
void allgreen(unsigned long period);
void allwhite(unsigned long period);
void allLEDs(unsigned long period);


void doPulseMode();
#endif // include guard
