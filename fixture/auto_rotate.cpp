#include "auto_rotate.h"


void doAutoRotate(){

	setSymmetryModeFromButtons();
	
	switch (g_symmetry){
	  
	  case TwoFold:
	    auto_rotate_twofold();
	    break;

	  case ThreeFold:
	    auto_rotate_threefold();
	    break;

	  case FiveFold:
	    auto_rotate_fivefold();
	    break;

	  case Reflect:
	    // auto_rotate_connected_cones();
	    break;
	}

}





void auto_rotate_twofold(){

	Serial.println(F("twofold auto_rotate"));
	
	// this is garbage because it doesn't always move forward, turing pot2 makes the animation jump.
	// but it's cool other than that
	uint16_t color_offset = positive_mod(getCurrentTime()*20*(float(pot2) / MAX_POT_VALUE),MAX_UINT16);


	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
	Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root


	// infer the second cone.  the node between them is on the line of symmetry
	Cone second_cone = get_connection(root_cone, connection_num); 

	if ( (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || g_previous_symmetry!=g_symmetry || previousEncoderValue!=encoder_counter){
	  getCurrentTime();
	  setStartTimeToNow();
	  setStartConeColorsFromCurrent();
	  setNextFrameTime(0);

	  current_cone[0] = root_cone;
	  current_cone[1] = second_cone;
	  current_cone[2] = connection_num;
	  
	  set_twofold_cycles(root_cone, second_cone);// set the appropriate cycles in the cycles array
	  set_twofold_colors_by_cycle_position(color_offset);//level();
	}

	
	transitionAllCones();

}	


void auto_rotate_threefold(){
	Serial.println(F("threefold auto_rotate"));

	uint16_t color_offset = positive_mod(getCurrentTime()*20*(float(pot2) / MAX_POT_VALUE),MAX_UINT16);

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.

	if ( (root_cone != current_cone[0]) || g_previous_symmetry!=g_symmetry || previousEncoderValue!=encoder_counter){
	  current_cone[0] = root_cone;

	  getCurrentTime();
	  setStartTimeToNow();
	  setStartConeColorsFromCurrent();
	  setNextFrameTime(0);
	  
	  set_threefold_cycles(root_cone);
	  // set the appropriate cycles in the cycles array
	  set_threefold_colors_by_cycle_position(color_offset);//level();
	}

	
	transitionAllCones();
}



void auto_rotate_fivefold(){
	Serial.println(F("fivefold auto_rotate"));

	uint16_t color_offset = positive_mod(getCurrentTime()*20*(float(pot2) / MAX_POT_VALUE),MAX_UINT16);

	Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
	Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root


	// infer the second cone.  the node between them is on the line of symmetry
	Cone second_cone = get_connection(root_cone, connection_num); 

	if ( (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || g_previous_symmetry!=g_symmetry || previousEncoderValue!=encoder_counter){
	  getCurrentTime();
	  setStartTimeToNow();
	  setStartConeColorsFromCurrent();
	  setNextFrameTime(0);

	  current_cone[0] = root_cone;
	  current_cone[1] = second_cone;
	  current_cone[2] = connection_num;
	  
	  set_fivefold_cycles(root_cone, second_cone, POSITIVE);// set the appropriate cycles in the cycles array
	  set_fivefold_colors_by_cycle_position(color_offset);
	}

	
	transitionAllCones();
}












