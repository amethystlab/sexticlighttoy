

#ifndef ROTATION_H
#define ROTATION_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else

#endif


#include "utility.h"
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


void printCycles();

uint8_t get_connection(uint8_t cone, uint8_t connection_num);

void set_twofold_cycles(Cone cone1, Cone cone2);
void set_threefold_cycles(Cone root_cone);
void set_fivefold_cycles();

void set_reflection_cycles();


uint8_t find_next_cone_in_direction(Cone src, Cone next, Direction dir);

void set_threefold(Cone src);
void set_fivefold(Cone *arr, Cone src, Cone next, Direction dir);


uint8_t get_mutual_connection(Cone coneOne, Cone coneTwo);
uint8_t find_connection_excluding_prev_cycles(uint8_t* arr, int cone, uint8_t min_avoid, uint8_t max_avoid);
uint8_t find_connection_index(Cone src, Cone conn);
Direction find_direction(Cone src, Cone first, Cone second);
Cone *find_middle_two(Cone coneOne, Cone coneTwo);
void set_cycle_presets();
void set_missing_in_cycles(uint8_t missingIndex);
void getNextPair(Cone *arr, Cone one, Cone two);


void incrementAxis(Direction dir);
void rotate(bool reverse);


void ThreeFoldRotateColor(Cone cone1, Cone cone2, Cone cone3);
void ThreeFoldSymmetry();
void nFoldRotateColor(Cone *coneArray, Cone numCone, bool reverse);
bool nFoldRotateColorTransition(Cone *coneArray, Cone numCone, bool reverse, uint16_t currentTime, uint16_t leng);

void rotationalThreeFoldSymEncoder();

void setSymmetryModeFromButtons();

void doRotationalMode();
void doReflectionalMode();
void doColorSetMode();
#endif // include guard
