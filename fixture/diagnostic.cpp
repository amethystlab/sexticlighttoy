#include "diagnostic.h"





void diagnostic_check_connected_cones(){
  Serial.println(F("diagnostic_check showing connected cones"));
  
  Cone& cone      = current_cone[0];
  Cone& prev_cone = current_cone[1];
  
  cone = positive_mod(rotary_counter,NUM_CONES);
  
  
  Serial.print(F("CURRENT NUM CONE: "));Serial.println(cone, DEC);

  uint32_t cone_tracker = (uint32_t(1) << cone);

  if (prev_cone!= cone){
    prev_cone = cone;

    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);


    // set the current cones to green
    setNextFrameColor(cone, GREEN);
    cone_tracker = cone_tracker | (uint32_t(1) << cone); // mark as used in the tracker, using bit ops

    // set the cones connected to the current cone red, record that they're used
    for(uint8_t i = 0; i < MAX_CONNECTION_NUM; ++i){
      Cone connected_cone = get_connection(cone, i);
      setNextFrameColor(connected_cone, RED);
      cone_tracker = cone_tracker | (uint32_t(1) << connected_cone); // mark that we used this cone.
    }
    
    // set the rest of the colors to black
    for (Cone i{0}; i<NUM_CONES; ++i){
      if (! (cone_tracker & (uint32_t(1) << i)) )
        setNextFrameColor(i, BLACK);
    }
  
  }

  transitionAllCones();
}





void diagnostic_check_twofold(){

  Serial.println(F("twofold diagnostic_check"));

  num_per_rotation = 2; // why is this global thing being set?  isn't it already set because the global mode is set?
  

  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
  Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root


  // infer the second cone.  the node between them is on the line of symmetry
  Cone second_cone = get_connection(root_cone, connection_num); 

  if ( (root_cone != current_cone[0]) || (connection_num != current_cone[2])){
    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);

    current_cone[0] = root_cone;
    current_cone[1] = second_cone;
    current_cone[2] = connection_num;
    
    set_twofold_cycles(root_cone, second_cone);// set the appropriate cycles in the cycles array
    set_twofold_colors_by_level();
  }

  
  transitionAllCones();
}


void set_twofold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  

  
  for (uint8_t level=0; level<7; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( 65535*(level+1.0)/7 ),255,value);
      setNextFrameColor(cycles[offset+j], color);
    }
    
  }
}



void set_twofold_colors_by_cycle_position(){
  pixels.clear();
  
  const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  

  
  for (uint8_t level=0; level<7; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( 65535*(j+1.0)/cycle_lengths[level] ),255,value);
      coneColor(cycles[offset+j], color);
    }
    
  }
}




void diagnostic_check_threefold(){

  Serial.println(F("threefold diagnostic_check"));

  num_per_rotation = 3; // why is this global thing being set?  isn't it already set because the global mode is set?



  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.

  if ( (root_cone != current_cone[0])){
    current_cone[0] = root_cone;

    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);
    
    set_threefold_cycles(root_cone);
    // set the appropriate cycles in the cycles array
    set_threefold_colors_by_level();
  }

  
  transitionAllCones();
}




void set_threefold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[8] = {1,3,3,3,3,3,3,1};
  

  
  for (uint8_t level=0; level<8; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( 65535*(level+1.0)/8 ),255,value);
      setNextFrameColor(cycles[offset+j], color);
    }
  }

}





void diagnostic_check_fivefold(){

  num_per_rotation = 5; // why is this global thing being set?  isn't it already set because the global mode is set?


  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
  Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root


  // infer the second cone.  the node between them is on the line of symmetry
  Cone second_cone = get_connection(root_cone, connection_num); 

  if ( (root_cone != current_cone[0]) || (connection_num != current_cone[2])){
    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);

    current_cone[0] = root_cone;
    current_cone[1] = second_cone;
    current_cone[2] = connection_num;
    
    set_fivefold_cycles(root_cone, second_cone, POSITIVE);// set the appropriate cycles in the cycles array
    set_fivefold_colors_by_level();
  }

  
  transitionAllCones();
}



void set_fivefold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[4] = {5,5,5,5};
  

  
  for (uint8_t level=0; level<4; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( 65535*(level+1.0)/8 ),255,value);
      setNextFrameColor(cycles[offset+j], color);
    }
  }

}






void doDiagnosticMode(){

  setSymmetryModeFromButtons();
  
  switch (symmetry){
    
    case TwoFold:
      diagnostic_check_twofold();
      break;

    case ThreeFold:
      diagnostic_check_threefold();
      break;

    case FiveFold:
      diagnostic_check_fivefold();
      break;

    case Reflect:
      diagnostic_check_connected_cones();
      break;
  }
    
}
