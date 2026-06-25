#include "manual_rotate.h"

#include "externs.h"


// turn the encoder to rotate the colors. pot2 sets how far each detent rotates.
// encoder motion is accumulated into a target (so pot2 only scales future
// detents, never moving colors already set), and the displayed offset glides
// toward that target -- the same time-based mechanism as auto-rotate -- so each
// step is a smooth sweep. returns the wrapped offset to render this frame.
uint16_t manual_color_offset(){
  int32_t step = MANUAL_ROTATE_STEP_MIN
               + (int32_t)(MANUAL_ROTATE_STEP_SPAN * (float(pot2) / MAX_POT_VALUE));

  // accumulate this frame's encoder motion, scaled by the current step. reject
  // implausibly large jumps: encoder_counter arrives over I2C and an occasional
  // corrupt reading would otherwise be integrated permanently (flash/revert).
  int32_t enc_delta = encoder_counter - g_manual_last_encoder;
  g_manual_last_encoder = encoder_counter;
  if (enc_delta <= MANUAL_ROTATE_MAX_DELTA && enc_delta >= -MANUAL_ROTATE_MAX_DELTA)
    g_manual_target += enc_delta * step;

  // never let the target lead the display by more than one detent's worth, so
  // the colors settle within one smooth step after you stop turning instead of
  // coasting onward (which confuses first-time users). a fast spin then just
  // rotates steadily while turning and stops promptly.
  float lead = (float)g_manual_target - g_manual_color_offset;
  if (lead >  step) g_manual_target = (int32_t)(g_manual_color_offset + step);
  if (lead < -step) g_manual_target = (int32_t)(g_manual_color_offset - step);

  // glide the displayed offset toward the target at a fixed rate (auto-rotate
  // style). rate-limiting also keeps any surviving glitch to a small nudge.
  Time delta_ms = g_current_time - g_previous_manual_rotate_time;
  g_previous_manual_rotate_time = g_current_time;
  if (delta_ms > AUTO_ROTATE_MAX_FRAME_MS) // e.g. just switched into this mode
    delta_ms = AUTO_ROTATE_MAX_FRAME_MS;

  float max_step = MANUAL_ROTATE_GLIDE_RATE * delta_ms;
  float diff = (float)g_manual_target - g_manual_color_offset;
  if (diff <= max_step && diff >= -max_step)
    g_manual_color_offset = g_manual_target;
  else
    g_manual_color_offset += (diff > 0) ? max_step : -max_step;

  return positive_mod((int32_t)g_manual_color_offset, MAX_UINT16);
}



void manual_rotate_connected_cones(){
  Serial.println(F("manual_rotate showing connected cones"));
  
  Cone& cone      = current_cone[0];
  Cone& prev_cone = current_cone[1];
  
  cone = positive_mod(encoder_counter,NUM_CONES);
  
  
  // Serial.print(F("CURRENT NUM CONE: "));Serial.println(cone, DEC);

  uint32_t cone_tracker = (uint32_t(1) << cone);

  if (prev_cone!= cone || g_previous_symmetry!=g_symmetry){
    prev_cone = cone;

    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(500*(1-float(pot2)/MAX_POT_VALUE));


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





void manual_rotate_twofold(bool highlight_axis){
  #ifdef DEBUG_PRINT
  Serial.println(F("twofold manual_rotate"));
  #endif
  
  getCurrentTime();
  uint16_t color_offset = manual_color_offset();

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

  // snap straight to the offset colors (no cross-fade); the encoder drives the
  // rotation directly.
  setStartTimeToNow();
  setNextFrameTime(0);
  set_twofold_colors_by_cycle_position(color_offset, highlight_axis);

  transitionAllCones();
}






void manual_rotate_threefold(bool highlight_axis){

  #ifdef DEBUG_PRINT
  Serial.println(F("threefold manual_rotate"));
  #endif

  getCurrentTime();
  uint16_t color_offset = manual_color_offset();

  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.

  bool need_compute_cycles = (root_cone != current_cone[0]) || g_previous_symmetry!=g_symmetry;

  if (need_compute_cycles){
    current_cone[0] = root_cone;
    set_threefold_cycles(root_cone);
  }

  // snap straight to the offset colors (no cross-fade); the encoder drives it.
  setStartTimeToNow();
  setNextFrameTime(0);
  set_threefold_colors_by_cycle_position(color_offset, highlight_axis);

  transitionAllCones();
}





void manual_rotate_fivefold(bool highlight_axis){

  #ifdef DEBUG_PRINT
  Serial.println(F("fivefold manual_rotate"));
  #endif

  getCurrentTime();
  uint16_t color_offset = manual_color_offset();

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

  // snap straight to the offset colors (no cross-fade); the encoder drives it.
  setStartTimeToNow();
  setNextFrameTime(0);
  set_fivefold_colors_by_cycle_position(color_offset, highlight_axis);

  transitionAllCones();
}










void set_twofold_colors_by_cycle_position(uint16_t color_offset, bool highlight_axis){
  // no pixels.clear() needed: every cone is assigned a color below, so the
  // whole strip is overwritten each frame by transitionAllCones().

  uint16_t hues[NUM_CONES];
  uint8_t saturations[NUM_CONES];

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    hues[ii] = positive_mod(twofold_angles[ii]/(2*pi)*MAX_UINT16+color_offset,MAX_UINT16);
  }

  if (highlight_axis){
    // desaturate only the axis cones (closest to the rotation axis) to white so
    // they stand out; every other cone stays fully saturated.
    float min_distance = twofold_distances[0];
    for (uint8_t ii{1}; ii<NUM_CONES; ++ii)
      if (twofold_distances[ii] < min_distance) min_distance = twofold_distances[ii];
    for (uint8_t ii{0}; ii<NUM_CONES; ++ii)
      saturations[ii] = (twofold_distances[ii] <= min_distance + 0.01) ? 0 : MAX_UINT8;
  }
  else
    for (uint8_t ii{0}; ii<NUM_CONES; ++ii)
      saturations[ii] = MAX_UINT8;


  uint8_t value = MAX_UINT8;

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    Color color = Adafruit_NeoPixel::ColorHSV(hues[ii],saturations[ii], value);
    setNextFrameColor(cycles[ii], color);

  }
}




void set_threefold_colors_by_cycle_position(uint16_t color_offset, bool highlight_axis){
  // no pixels.clear() needed: every cone is assigned a color below.
  // const uint8_t cycle_lengths[8] = {1,3,3,3,3,3,3,1};




  uint16_t hues[NUM_CONES];
  uint8_t saturations[NUM_CONES];

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    hues[ii] = positive_mod(threefold_angles[ii]/(2*pi)*MAX_UINT16+color_offset,MAX_UINT16);
  }

  if (highlight_axis){
    // desaturate only the axis cones (closest to the rotation axis) to white so
    // they stand out; every other cone stays fully saturated.
    float min_distance = threefold_distances[0];
    for (uint8_t ii{1}; ii<NUM_CONES; ++ii)
      if (threefold_distances[ii] < min_distance) min_distance = threefold_distances[ii];
    for (uint8_t ii{0}; ii<NUM_CONES; ++ii)
      saturations[ii] = (threefold_distances[ii] <= min_distance + 0.01) ? 0 : MAX_UINT8;
  }
  else
    for (uint8_t ii{0}; ii<NUM_CONES; ++ii)
      saturations[ii] = MAX_UINT8;

  uint8_t value = MAX_UINT8;

  for (uint8_t ii=0; ii<NUM_CONES; ++ii){
    Color color = Adafruit_NeoPixel::ColorHSV( hues[ii],saturations[ii],value);
    setNextFrameColor(cycles[ii], color);
  }

}





void set_fivefold_colors_by_cycle_position(uint16_t color_offset, bool highlight_axis){
  // no pixels.clear() needed: every cone is assigned a color below.
  const uint8_t cycle_lengths[4] = {5,5,5,5};
  


  

  uint16_t hues[NUM_CONES];
  uint8_t saturations[NUM_CONES];


  if (highlight_axis)
  {
    for (uint8_t ii{0}; ii<5; ++ii)
      saturations[ii] = 0;
    for (uint8_t ii{5}; ii<10; ++ii)
      saturations[ii] = MAX_UINT8;
    for (uint8_t ii{10}; ii<15; ++ii)
      saturations[ii] = MAX_UINT8;
    for (uint8_t ii{15}; ii<20; ++ii)
      saturations[ii] = 0;
  }
  else
    for (uint8_t ii{0}; ii<NUM_CONES; ++ii)
      saturations[ii] = MAX_UINT8;


  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    hues[ii] = positive_mod(fivefold_angles[ii]/(2*pi)*MAX_UINT16+color_offset,MAX_UINT16);
  }

  uint8_t value = MAX_UINT8;

  for (uint8_t ii=0; ii<NUM_CONES; ++ii){
    Color color = Adafruit_NeoPixel::ColorHSV( hues[ii],saturations[ii],value);
    setNextFrameColor(cycles[ii], color);
  }


}






void set_twofold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  

  
  for (uint8_t level=0; level<7; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( MAX_UINT16*(level+1.0)/7 ),255,value);
      setNextFrameColor(cycles[offset+j], color);
    }
    
  }
}


void set_threefold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[8] = {1,3,3,3,3,3,3,1};
  

  
  for (uint8_t level=0; level<8; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? MAX_UINT8 : (MAX_UINT8/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( MAX_UINT16*(level+1.0)/8 ),MAX_UINT8,value);
      setNextFrameColor(cycles[offset+j], color);
    }
  }

}



void set_fivefold_colors_by_level(){
  pixels.clear();
  
  const uint8_t cycle_lengths[4] = {5,5,5,5};
  

  
  for (uint8_t level=0; level<4; ++level){
    uint8_t offset = partial_sum(cycle_lengths,level);
    
    for (uint8_t j=0; j<cycle_lengths[level]; ++j){
      uint8_t value = (level==0 ? 255 : (255/4));
      Color color = Adafruit_NeoPixel::ColorHSV(uint16_t( MAX_UINT16*(level+1.0)/8 ),255,value);
      setNextFrameColor(cycles[offset+j], color);
    }
  }

}





void doManualRotate(bool highlight_axis){
  // test_hsvrgb();

  setSymmetryModeFromButtons();
  
  switch (g_symmetry){
    
    case TwoFold:
      manual_rotate_twofold(highlight_axis);
      break;

    case ThreeFold:
      manual_rotate_threefold(highlight_axis);
      break;

    case FiveFold:
      manual_rotate_fivefold(highlight_axis);
      break;

    case Reflect:
      manual_rotate_connected_cones();
      break;
  }
  
}
