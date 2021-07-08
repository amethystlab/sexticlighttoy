#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H


#include "rotation.h"

void diagnostic_check();

void diagnostic_check_connected_cones();
void diagnostic_check_threefold();
void diagnostic_check_fivefold();
void diagnostic_check_twofold();
void diagnostic_check_find_third();

void set_twofold_colors_by_level();
void set_twofold_colors_by_cycle_position();
void set_threefold_colors_by_level();
void set_fivefold_colors_by_level();

void doDiagnosticMode();
#endif
