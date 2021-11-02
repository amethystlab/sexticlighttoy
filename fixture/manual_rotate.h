#ifndef MANUAL_ROTATE_H
#define MANUAL_ROTATE_H

#include "externs.h"
#include "rotation.h"

void manual_rotate_connected_cones();
void manual_rotate_threefold();
void manual_rotate_fivefold();
void manual_rotate_twofold();


void set_twofold_colors_by_level();
void set_twofold_colors_by_cycle_position(uint16_t color_offset);
void set_threefold_colors_by_level();
void set_threefold_colors_by_cycle_position(uint16_t color_offset);

void set_fivefold_colors_by_level();
void set_fivefold_colors_by_cycle_position(uint16_t color_offset);

void doManualRotate();
#endif
