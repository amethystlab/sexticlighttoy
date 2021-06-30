

#ifndef ROTATION_H
#define ROTATION_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif


#include "externs.h"
#include "stack.h"
#include "color.h"
#include "device.h"


//uint8_t threeFoldGroups [6][3] = {{5, 14, 2}, {15, 12, 4}, {6, 13, 3}, {7, 20, 10}, {8, 16, 11}, {17, 19, 9}}; //[6] indicates amount of arrays, [3] arrays in the array
//uint8_t fiveFoldGroups [4][5] = {{1,2,3,4,5}, {14,12,10,8,6}, {9,7,15,13,11}, {18,17,16,20,19}}; //the numbering of these groups are in clockwise-order starting at "top" cone

//{1,2,3,4,5} = r,g,b,w,y
//{14,12,10,8,6} = r,g,b,w,y
//{9,7,15,13,11} = r,g,b,w,y
//{18,17,16,20,19} = r,g,b,w,y


uint8_t get_connection(uint8_t cone, uint8_t connection_num);

void set_threefold_cycles();
void set_pentagon_cycles();
void set_twofold_cycles();

void set_reflection_cycles();


uint8_t findThird(uint8_t src, uint8_t next, uint8_t dir);

void set_twofold(uint8_t one, uint8_t two);
void set_threefold(uint8_t src);
void set_pentagon(uint8_t *arr, uint8_t src, uint8_t next, uint8_t dir);


uint8_t get_mutual_connection(uint8_t coneOne, uint8_t coneTwo);
uint8_t find_connection_excluding_prev_cycles(uint8_t* arr, int cone, uint8_t min_avoid, uint8_t max_avoid);
uint8_t find_connection_index(uint8_t src, uint8_t conn);
uint8_t find_direction(uint8_t src, uint8_t first, uint8_t second);
uint8_t *find_middle_two(uint8_t coneOne, uint8_t coneTwo);
void set_cycle_presets();
void set_missing_in_cycles(uint8_t missingIndex);
void getNextPair(uint8_t *arr, uint8_t one, uint8_t two);


void incrementAxis(int8_t dir);
void rotate(bool reverse);


void ThreeFoldRotateColor(uint8_t cone1, uint8_t cone2, uint8_t cone3);
void ThreeFoldSymmetry();
void nFoldRotateColor(uint8_t *coneArray, uint8_t numCone, bool reverse);
bool nFoldRotateColorTransition(uint8_t *coneArray, uint8_t numCone, bool reverse, uint16_t currentTime, uint16_t leng);

void rotationalThreeFoldSymEncoder();

void doRotationalMode();
void doReflectionalMode();
void doColorSetMode();
#endif // include guard
