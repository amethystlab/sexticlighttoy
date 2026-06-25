#ifndef AUTO_ROTATE_H
#define AUTO_ROTATE_H


#include "manual_rotate.h"



// advances g_auto_color_rotation based on elapsed wall-clock time (not frame
// count) so the rotation speed is independent of frame rate. requires
// g_current_time to be current (call getCurrentTime() first).
void advance_auto_color_rotation();

void auto_rotate_twofold(bool highlight_axis);
void auto_rotate_threefold(bool highlight_axis);
void auto_rotate_fivefold(bool highlight_axis);




// contains the code to select the symmetry you want.
void doAutoRotate(bool highlight_axis);



#endif

