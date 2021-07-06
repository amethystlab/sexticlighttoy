#include "utility.h"

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
