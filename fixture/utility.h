
#ifndef UTILITY_H
#define UTILITY_H


#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#endif

unsigned positive_mod( int32_t value, unsigned modulus);
unsigned positive_div( int32_t value, unsigned denom);

#endif
