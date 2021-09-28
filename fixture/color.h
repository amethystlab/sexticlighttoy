

#ifndef COLOR_H
#define COLOR_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif

#include "externs.h"


/////// from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
// David H.'s code for this problem.  
struct RGB {
    double red;       // a fraction between 0 and 1
    double green;       // a fraction between 0 and 1
    double blue;       // a fraction between 0 and 1

    RGB(){}

    RGB(uint8_t r,uint8_t g,uint8_t b){
        red = double(r)/MAX_UINT8;
        green = double(g)/MAX_UINT8;
        blue = double(b)/MAX_UINT8;
    }

    inline uint8_t RedAsInt() const{return red*MAX_UINT8;}
    inline uint8_t BlueAsInt() const{return blue*MAX_UINT8;}
    inline uint8_t GreenAsInt() const{return green*MAX_UINT8;}

    inline void Print() const{Serial.print(red);Serial.print(" ");Serial.print(green);Serial.print(" ");Serial.println(blue);}
};

/////// from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
// David H.'s code for this problem.  
struct HSV{
    double hue;       // angle in degrees
    double saturation;       // a fraction between 0 and 1
    double value;       // a fraction between 0 and 1

    HSV(){}

    HSV(uint16_t h,uint8_t s,uint8_t v){
        hue = double(h)/MAX_UINT16*360.0;
        saturation = double(s)/MAX_UINT8;
        value = double(v)/MAX_UINT8;
    }

    inline
    uint16_t HueAsInt() const{return hue/360.*MAX_UINT16;}

    inline 
    uint8_t SatAsInt() const{return saturation * MAX_UINT8;}

    inline 
    uint8_t ValAsInt() const{return value * MAX_UINT8;}
};


HSV   rgb2hsv(RGB in);
RGB   hsv2rgb(HSV in);

void test_hsvrgb();

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


#endif // include guard
