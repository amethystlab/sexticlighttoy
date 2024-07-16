#include "auto_rotate.h"


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

	Serial.println(F("twofold auto_rotate"));
	

	g_auto_color_rotation = g_auto_color_rotation + 2000*(float(pot2) / MAX_POT_VALUE) + 100;
	g_auto_color_rotation = positive_mod(g_auto_color_rotation, MAX_UINT16);

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
	Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root

	// infer the second cone.  the node between them is on the line of symmetry
	Cone second_cone = get_connection(root_cone, connection_num); 

	bool need_compute_cycles = (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || g_previous_symmetry!=g_symmetry;
	bool need_compute_colors = true;

	if (need_compute_cycles){
		current_cone[0] = root_cone;
		current_cone[1] = second_cone;
		current_cone[2] = connection_num;

		set_twofold_cycles(root_cone, second_cone);// set the appropriate cycles in the cycles array
	}

	if (need_compute_colors){
	  getCurrentTime();
	  setStartTimeToNow();
	  setStartConeColorsFromCurrent();
	  setNextFrameTime(0);

	  set_twofold_colors_by_cycle_position(g_auto_color_rotation, highlight_axis);
	}

	
	transitionAllCones();

}	


void auto_rotate_threefold(bool highlight_axis){
	Serial.println(F("threefold auto_rotate"));

	g_auto_color_rotation = g_auto_color_rotation + 2000*(float(pot2) / MAX_POT_VALUE) + 100;
	g_auto_color_rotation = positive_mod(g_auto_color_rotation, MAX_UINT16);

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.

	bool need_compute_cycles = (root_cone != current_cone[0]) || g_previous_symmetry!=g_symmetry;
	bool need_compute_colors = true;

	if (need_compute_cycles){
		current_cone[0] = root_cone;
		set_threefold_cycles(root_cone);
	}

	if (need_compute_colors){
	  
	  getCurrentTime();
	  setStartTimeToNow();
	  setStartConeColorsFromCurrent();
	  setNextFrameTime(0);
	  
	  // set the appropriate cycles in the cycles array
	  set_threefold_colors_by_cycle_position(g_auto_color_rotation, highlight_axis);
	}

	
	transitionAllCones();
}



void auto_rotate_fivefold(bool highlight_axis){
	Serial.println(F("fivefold auto_rotate"));

	g_auto_color_rotation = g_auto_color_rotation + 2000*(float(pot2) / MAX_POT_VALUE) + 100;
	g_auto_color_rotation = positive_mod(g_auto_color_rotation, MAX_UINT16);

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
	Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root
	// infer the second cone.  the node between them is on the line of symmetry
	Cone second_cone = get_connection(root_cone, connection_num); 

	bool need_compute_cycles = (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || g_previous_symmetry!=g_symmetry;
	bool need_compute_colors = true;

	if (need_compute_cycles){
		current_cone[0] = root_cone;
		current_cone[1] = second_cone;
		current_cone[2] = connection_num;
		
		set_fivefold_cycles(root_cone, second_cone, POSITIVE);// set the appropriate cycles in the cycles array
	}

	if (need_compute_colors){
	  getCurrentTime();
	  setStartTimeToNow();
	  setStartConeColorsFromCurrent();
	  setNextFrameTime(0);

	  set_fivefold_colors_by_cycle_position(g_auto_color_rotation, highlight_axis);
	}

	
	transitionAllCones();
}












