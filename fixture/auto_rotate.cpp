#include "auto_rotate.h"


// advance the color rotation by an amount proportional to the time elapsed
// since the previous frame, so the visible speed does not change when the
// frame rate does.  pot2 scales the rate; see AUTO_ROTATE_HUE_RATE_* in defines.h.
void advance_auto_color_rotation(){
	Time delta_ms = g_current_time - g_previous_auto_rotate_time;
	g_previous_auto_rotate_time = g_current_time;

	if (delta_ms > AUTO_ROTATE_MAX_FRAME_MS) // e.g. just switched into this mode
		delta_ms = AUTO_ROTATE_MAX_FRAME_MS;

	float rate = AUTO_ROTATE_HUE_RATE_MIN + AUTO_ROTATE_HUE_RATE_SPAN * (float(pot2) / MAX_POT_VALUE);
	g_auto_color_rotation = positive_mod(g_auto_color_rotation + (int32_t)(rate * delta_ms), MAX_UINT16);
}


void doAutoRotate(bool highlight_axis){

	setSymmetryModeFromButtons();
	
	switch (g_symmetry){
	  
	  case TwoFold:
	    auto_rotate_twofold(highlight_axis);
	    break;

	  case ThreeFold:
	    auto_rotate_threefold(highlight_axis);
	    break;

	  case FiveFold:
	    auto_rotate_fivefold(highlight_axis);
	    break;

	  case Reflect:
	    // auto_rotate_connected_cones();
	    break;
	}

}





void auto_rotate_twofold(bool highlight_axis){
#ifdef DEBUG_PRINT
	Serial.println(F("twofold auto_rotate"));
#endif

	getCurrentTime();
	advance_auto_color_rotation();

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
	Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root

	// infer the second cone.  the node between them is on the line of symmetry
	Cone second_cone = get_connection(root_cone, connection_num);

	bool need_compute_cycles = (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || g_previous_symmetry!=g_symmetry;

	if (need_compute_cycles){
		current_cone[0] = root_cone;
		current_cone[1] = second_cone;
		current_cone[2] = connection_num;

		set_twofold_cycles(root_cone, second_cone);// set the appropriate cycles in the cycles array
	}

	// zero-duration "transition": cones snap straight to the cycle colors.
	setStartTimeToNow();
	setNextFrameTime(0);
	set_twofold_colors_by_cycle_position(g_auto_color_rotation, highlight_axis);

	transitionAllCones();

}


void auto_rotate_threefold(bool highlight_axis){
#ifdef DEBUG_PRINT
	Serial.println(F("threefold auto_rotate"));
#endif

	getCurrentTime();
	advance_auto_color_rotation();

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.

	bool need_compute_cycles = (root_cone != current_cone[0]) || g_previous_symmetry!=g_symmetry;

	if (need_compute_cycles){
		current_cone[0] = root_cone;
		set_threefold_cycles(root_cone);
	}

	// zero-duration "transition": cones snap straight to the cycle colors.
	setStartTimeToNow();
	setNextFrameTime(0);
	set_threefold_colors_by_cycle_position(g_auto_color_rotation, highlight_axis);

	transitionAllCones();
}



void auto_rotate_fivefold(bool highlight_axis){
#ifdef DEBUG_PRINT
	Serial.println(F("fivefold auto_rotate"));
#endif

	getCurrentTime();
	advance_auto_color_rotation();

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
	Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root
	// infer the second cone.  the node between them is on the line of symmetry
	Cone second_cone = get_connection(root_cone, connection_num);

	bool need_compute_cycles = (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || g_previous_symmetry!=g_symmetry;

	if (need_compute_cycles){
		current_cone[0] = root_cone;
		current_cone[1] = second_cone;
		current_cone[2] = connection_num;

		set_fivefold_cycles(root_cone, second_cone, POSITIVE);// set the appropriate cycles in the cycles array
	}

	// zero-duration "transition": cones snap straight to the cycle colors.
	setStartTimeToNow();
	setNextFrameTime(0);
	set_fivefold_colors_by_cycle_position(g_auto_color_rotation, highlight_axis);

	transitionAllCones();
}












