#include "utility.h"


uint8_t partial_sum(uint8_t* data, uint8_t num){
  uint8_t result = 0;
  for (uint8_t i=0; i<num; ++i)
    result += data[i];
  return result;
}


unsigned positive_mod( int32_t value, unsigned modulus) {
    int32_t result = value % (int32_t)modulus;
    if (result < 0) {
        result += modulus;
    }
    return result;
}


unsigned positive_div( int32_t value, unsigned denom) {
    int32_t result = value / (int32_t)denom;
    if (result < 0) {
        result += denom;
    }
    return result;
}
