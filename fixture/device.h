#ifndef DEVICE_H
#define DEVICE_H

#include <Wire.h> // see https://www.arduino.cc/en/Tutorial/MasterReader

#include "externs.h"
#include "color.h"
void receiveEvent(int howMany);
void get_encoder();
void get_buttons();
void get_switches();
void get_pots();
void print_state();
uint16_t get_uint16();
int32_t get_int32();

void coneSelect();
void reset();

void getMode();

void setupPixels();
void setupCommunication();
void setupSerial();
void setupTime();
#endif
