#include "diagnostic.h"


void diagnostic_check_connected_cones(){
  Serial.println(F("diagnostic_check showing connected cones"));

  Cone& cone      = current_cone[0];
  Cone& prev_cone = current_cone[1];
  
  cone = positive_mod(rotary_counter,NUM_CONES);

  // Serial.print("CURRENT NUM CONE: ");
  // Serial.println(cone, DEC);
  
  // turn off all cones
  for (int i=0; i<NUM_CONES; ++i)
    coneColor(i, BLACK);
  
  
  // Light up the current cone (cone number cone) green
  coneColor(cone, GREEN);
  
  Serial.print(F("CONNECTED CONES: "));
  for(int i = 0; i < MAX_CONNECTION_NUM; ++i){
    // Light up the cones connected to the current cone red
    uint8_t connected_cone = get_connection(cone, i);

    // Serial.println(connected_cone, DEC);
    
    coneColor(connected_cone, RED);
  }
  pixels.show();
}



void diagnostic_check_connected_cones_using_events(){
  Serial.println(F("frame-driven diagnostic_check showing connected cones"));
  
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
    setNextFrameTime(10000*float(pot3)/MAX_POT_VALUE);


    // set the current cones to green
    setNextFrameColor(cone, GREEN);
    cone_tracker = cone_tracker | (uint32_t(1) << cone); // mark as used in the tracker, using bit ops

    // set the cones connected to the current cone red, record that they're used
    for(int i = 0; i < MAX_CONNECTION_NUM; ++i){
      Cone connected_cone = get_connection(cone, i);
      setNextFrameColor(connected_cone, RED);
      cone_tracker = cone_tracker | (uint32_t(1) << connected_cone); // mark that we used this cone.
    }
    
    // set the rest of the colors to black
    for (uint8_t i{0}; i<NUM_CONES; ++i){
      if (! (cone_tracker & (uint32_t(1) << i)) )
        setNextFrameColor(i, BLACK);
    }
  
  }

  transitionAllCones();
}





void diagnostic_check_twofold(){

  // Serial.println("twofold diagnostic_check");
  num_per_rotation = 2;
  
  Cone& root_cone = current_cone[0];
  Cone& second_cone = current_cone[1];
  uint8_t& connection_num = current_cone[2];
  
  connection_num = positive_mod(rotary_counter,3); // indexes the connected cones to the root
  root_cone = positive_div(rotary_counter,20); // the active root of the rotation.
  second_cone = get_connection(root_cone, connection_num); // the second cone.  the node between them is on the line of symmetry


  
  if (previousEncoderValue!=rotary_counter){
    previousEncoderValue = rotary_counter;

    // Serial.println("changing line of symmetry");
    // 
    // Serial.print("encoder, previous encoder: "); Serial.print(rotary_counter);Serial.println(previousEncoderValue);
    // Serial.print("root cone, second cone: "); Serial.print(root_cone); Serial.println(second_cone);
    
    connection_num = positive_mod(rotary_counter,3);
    root_cone = positive_div(rotary_counter,20);
    
    second_cone = get_connection(root_cone, connection_num);
    
    set_twofold_cycles(root_cone, second_cone);// set the appropriate cycles in the cycles array
  }


  //   // by default, perform a "rotation" of the lights about the axis
  //   Serial.println("performing a rotation");
  // 
  //   rotate(false);
  //   pixels.show();
  
  // Serial.print("root cone, second cone, connection_num: "); 
  // Serial.print(root_cone); Serial.print(" "); Serial.print(second_cone); Serial.print(" "); Serial.print(connection_num);  Serial.print("\n");


  set_twofold_colors_by_cycle_position();
  pixels.show(); 
  
  // delay(500);
}


void set_twofold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  

  
  for (uint8_t level=0; level<7; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( 65535*(level+1.0)/7 ),255,value);
      coneColor(cycles[offset+j], color);
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
  num_per_rotation = 3;

  static int num = 1;
  static bool prevState = false;
  static bool prevStateTwo = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  if(is_switch_on(0) != prevState){
    // if we flip the first switch
    
    prevState = is_switch_on(0);
    clear();

    num = (num % NUM_LED_GROUPS) + 1;
    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
    
    set_threefold(num);
    // set the axis cone in the cycles array
    set_threefold_cycles();
    // set the appropriate cycles in the cycles array

#ifdef DEBUG_PRINT
    Serial.print(F("Cycles: "));
    for(int i = 0; i < 20; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(F(", "));
    }
    Serial.println("");
#endif

    set_cycle_presets();
    // color the cones appropriately


    pixels.show();

    
  }

  if(is_switch_on(1) != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    rotate(false);
    pixels.show();
  }

}





void diagnostic_check_fivefold(){
  Serial.println(F("fivefold diagnostic_check"));
  symmetry = FiveFold;
  num_per_rotation = 5;
  
  static int num = 1;
  static bool prevState = false;
  static bool prevStateTwo = false;
  static bool prevStateThree = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  if(is_switch_on(0) != prevState){
    // if we flip the first switch
    
    prevState = is_switch_on(0);
    clear();

    num = (num % NUM_LED_GROUPS) + 1;
    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
#ifdef DEBUG_PRINT
    Serial.print(F("NUM CONE: "));
    Serial.println(num, DEC);
#endif

    uint8_t next = get_connection(num, 0);
    set_fivefold(cycles, num, next, POSITIVE);
    // set the axis pentagon in the cycles array

    set_fivefold_cycles();
    // set the appropriate cycles in the cycles array
#ifdef DEBUG_PRINT
    Serial.print(F("PENTAGON: "));
    for(int i = 0; i < 5; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");
    Serial.print(F("Cycles: "));
    for(int i = 0; i < 20; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");
#endif

    set_cycle_presets();
    // color the cones in the cycles appropriately
    
    pixels.show(); 
  }

  if(is_switch_on(1) != prevStateTwo){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    prevStateTwo = is_switch_on(1);
    rotate(false);
    pixels.show();
  }

  if(is_switch_on(2) != prevStateThree){
    // if the second switch changes positions, perform
    // a 3 fold "rotation" of the lights about the axis
    prevStateThree = is_switch_on(2);
    rotate(true);
    pixels.show();
  }

}


void diagnostic_check_find_third(){
  static uint8_t src = 1;
  static uint8_t next = 8;
  
  static bool prevState = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  //uint8_t findThird(uint8_t src, uint8_t next, uint8_t dir){


  if(is_switch_on(0) != prevState){
    // if we flip the first switch
    clear();
    uint8_t arr[5];
    set_fivefold(arr, 1, 2, POSITIVE);
    
    for(int i = 0; i < 5; i++) coneColor(arr[i], 255,255,255,255);
    
//    uint8_t third = findThird(src, next, POSITIVE);
//    next = src;
//    src = third;
//
//    coneColor(src, 255, 0, 255, 255);
//    coneColor(next, 0, 255, 255, 255);
//
//    Serial.print("src: "); Serial.print(src); Serial.print(" next: "); Serial.println(next);

    pixels.show();

    prevState = is_switch_on(0);
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
      diagnostic_check_connected_cones_using_events();
      break;
  }
    
}
