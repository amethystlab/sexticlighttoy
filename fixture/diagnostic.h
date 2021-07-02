#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H


#include "rotation.h"

void diagnostic_check();
void diagnostic_check_threefold();
void diagnostic_check_fivefold();
void diagnostic_check_twofold();
void diagnostic_check_find_third();

void set_twofold_colors_from_cycles();

void doDiagnosticMode();
#endif
