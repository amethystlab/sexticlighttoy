
#ifndef UTILITY_H
#define UTILITY_H


#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#endif

uint8_t partial_sum(uint8_t* data, uint8_t num);
unsigned positive_mod( int32_t value, unsigned modulus);
unsigned positive_div( int32_t value, unsigned denom);

#endif
