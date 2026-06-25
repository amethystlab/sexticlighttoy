#ifndef MANUAL_ROTATE_H
#define MANUAL_ROTATE_H

#include "externs.h"
#include "rotation.h"

// maps the encoder position to a hue offset, with pot2 setting the per-detent
// angle delta. stateless, so a glitched I2C encoder reading self-corrects.
uint16_t manual_color_offset();

void manual_rotate_connected_cones();
void manual_rotate_threefold();
void manual_rotate_fivefold();
void manual_rotate_twofold();


void set_twofold_colors_by_level();
void set_twofold_colors_by_cycle_position(uint16_t color_offset, bool highlight_axis);
void set_threefold_colors_by_level();
void set_threefold_colors_by_cycle_position(uint16_t color_offset, bool highlight_axis);

void set_fivefold_colors_by_level();
void set_fivefold_colors_by_cycle_position(uint16_t color_offset, bool highlight_axis);

void doManualRotate(bool highlight_axis);
#endif
