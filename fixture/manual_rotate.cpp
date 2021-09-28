#include "manual_rotate.h"





void manual_rotate_connected_cones(){
  Serial.println(F("manual_rotate showing connected cones"));
  
  Cone& cone      = current_cone[0];
  Cone& prev_cone = current_cone[1];
  
  cone = positive_mod(rotary_counter,NUM_CONES);
  
  
  // Serial.print(F("CURRENT NUM CONE: "));Serial.println(cone, DEC);

  uint32_t cone_tracker = (uint32_t(1) << cone);

  if (prev_cone!= cone || previous_symmetry!=symmetry){
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





void manual_rotate_twofold(){

  Serial.println(F("twofold manual_rotate"));
  
  uint16_t color_offset = positive_mod(rotary_counter*1000,MAX_UINT16);

  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
  Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root


  // infer the second cone.  the node between them is on the line of symmetry
  Cone second_cone = get_connection(root_cone, connection_num); 

  if ( (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || previous_symmetry!=symmetry || previousEncoderValue!=rotary_counter){
    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);

    current_cone[0] = root_cone;
    current_cone[1] = second_cone;
    current_cone[2] = connection_num;
    
    set_twofold_cycles(root_cone, second_cone);// set the appropriate cycles in the cycles array
    set_twofold_colors_by_cycle_position(color_offset);//level();
  }

  
  transitionAllCones();
}






void manual_rotate_threefold(){

  Serial.println(F("threefold manual_rotate"));

  uint16_t color_offset = positive_mod(rotary_counter*1000,MAX_UINT16);

  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.

  if ( (root_cone != current_cone[0]) || previous_symmetry!=symmetry || previousEncoderValue!=rotary_counter){
    current_cone[0] = root_cone;

    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);
    
    set_threefold_cycles(root_cone);
    // set the appropriate cycles in the cycles array
    set_threefold_colors_by_cycle_position(color_offset);//level();
  }

  
  transitionAllCones();
}





void manual_rotate_fivefold(){

  Serial.println(F("fivefold manual_rotate"));

  uint16_t color_offset = positive_mod(rotary_counter*1000,MAX_UINT16);

  Cone root_cone = (NUM_CONES-1)*float(pot0)/MAX_POT_VALUE; // the active root of the rotation.
  Connection connection_num = (MAX_CONNECTION_NUM-1)*float(pot1)/MAX_POT_VALUE; // indexes the connected cones to the root


  // infer the second cone.  the node between them is on the line of symmetry
  Cone second_cone = get_connection(root_cone, connection_num); 

  if ( (root_cone != current_cone[0]) || (connection_num != current_cone[2]) || previous_symmetry!=symmetry || previousEncoderValue!=rotary_counter){
    getCurrentTime();
    setStartTimeToNow();
    setStartConeColorsFromCurrent();
    setNextFrameTime(10000*float(pot2)/MAX_POT_VALUE);

    current_cone[0] = root_cone;
    current_cone[1] = second_cone;
    current_cone[2] = connection_num;
    
    set_fivefold_cycles(root_cone, second_cone, POSITIVE);// set the appropriate cycles in the cycles array
    set_fivefold_colors_by_cycle_position(color_offset);
  }

  
  transitionAllCones();
}










void set_twofold_colors_by_cycle_position(uint16_t color_offset){
  pixels.clear();
  
  // const uint8_t cycle_lengths[7] = {2,4,2,4,2,4,2};
  

  // these magic constants computed with the help of python.
  // i wrote a notebook to compute them, based on 
  // rotating the points on a dodecahedron
  // and projecting them to the xy plane,
  // then using atan2, my favorite lil function.
  // this array should be computed in memory, instead of every time this
  // function hits.  when you next read this, please 
  // make this modification.  
  float angles[NUM_CONES] = {0,pi,   
                            3*pi/4 , 5*pi/4      ,  7*pi/4   , pi/4,
                            pi/2, 3*pi/2,     
                            -2.7767288254763103+2*pi,2.7767288254763103,0.3648638281134831,-0.3648638281134831+2*pi, // ,   
                            pi/2, 3*pi/2,
                            pi/4, 3*pi/4, 5*pi/4, 7*pi/4, 
                            pi,0
                          };

  const float distances[NUM_CONES] = {phi-1, phi-1,
                                sqrt(2),sqrt(2),sqrt(2),sqrt(2),
                                phi,phi,
                                sqrt(3),sqrt(3),sqrt(3),sqrt(3),
                                phi,phi,
                                sqrt(2),sqrt(2),sqrt(2),sqrt(2),
                                phi-1, phi-1
                              };

  uint16_t hues[NUM_CONES];
  uint8_t saturations[NUM_CONES];

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    hues[ii] = positive_mod(angles[ii]/(2*pi)*MAX_UINT16+color_offset,MAX_UINT16);
  }

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    saturations[ii] = (distances[ii]-(phi-1))/(sqrt(3)-(phi-1)) * MAX_UINT8;
  }

  uint8_t value = MAX_UINT8;

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    Color color = Adafruit_NeoPixel::ColorHSV(hues[ii],saturations[ii], value);
    setNextFrameColor(cycles[ii], color);

  }
}




void set_threefold_colors_by_cycle_position(uint16_t color_offset){
  pixels.clear();
  
  const uint8_t cycle_lengths[8] = {1,3,3,3,3,3,3,1};
  
  float a = 0.6590580358263686; // 37.76 deg, green-yellow
  float b = 1.435337066566721;  // 82.24 deg, orange probably, to yellow
  float c = 0.3881395153702137; // 22.24 deg, green, to slightly yellow
  float d = 1.7062555870230722; // 97.76 deg, reddish

  float e = pi/3;


  float s2 = 2*pi/3;
  float s4 = 4*pi/3;
  
  // correct if the root node is 17. also appears correct for all roots.

  const float angles[NUM_CONES] = {0, // ill-defined angle, so 0

                                   0, s2, s4,    // easily seen equilateral triangle directly adjacent to root cone

                                   b, b+s2, b+s4,  
                                   a, a+s2, a+s4,   // two slightly shifted triangles, distance 2 from root cone
                                   
                                   
                                   c+s2, c+s4, c, // analagous structures on bottom side (root is top)
                                   d,d+s2, d+s4, 

                                   e+s2, e+s4, e, // easily seen equilateral triangle directly adjacent to root cone

                                   0 // ill-defined angle, so 0
                                 };

  const float distances[NUM_CONES] = {0,
                                      1.1547005383791311,1.1547005383791311,1.1547005383791311,
                                      
                                      1.6329931618555373,1.6329931618555373,1.6329931618555373,
                                      1.6329931618555373,1.6329931618555373,1.6329931618555373,

                                      1.6329931618555373,1.6329931618555373,1.6329931618555373,
                                      1.6329931618555373,1.6329931618555373,1.6329931618555373,

                                      1.1547005383791311,1.1547005383791311,1.1547005383791311,

                                      0
                                    };

  uint16_t hues[NUM_CONES];
  uint8_t saturations[NUM_CONES];

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    hues[ii] = positive_mod(angles[ii]/(2*pi)*MAX_UINT16+color_offset,MAX_UINT16);
  }

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    saturations[ii] = distances[ii]/1.6329931618555373 * MAX_UINT8;
  }

  uint8_t value = MAX_UINT8;

  for (uint8_t ii=0; ii<NUM_CONES; ++ii){
    Color color = Adafruit_NeoPixel::ColorHSV( hues[ii],saturations[ii],value);
    setNextFrameColor(cycles[ii], color);
  }

}





void set_fivefold_colors_by_cycle_position(uint16_t color_offset){
  pixels.clear();
  
  const uint8_t cycle_lengths[4] = {5,5,5,5};
  

  const float a = 1.2566370614359172; // 72 degrees
  const float b = a/2; // 36 degrees -- the pentagons on the bottom half are rotated to bisect the angle

  const float angles[NUM_CONES] = {0,a,a+a,3*a, 4*a, // these are the same, amazingly enough
                                   0,a,a+a,3*a, 4*a,
                                   b,a+b,2*a+b, 3*a+b, 4*a+b,
                                   b,a+b,2*a+b, 3*a+b, 4*a+b
                                  };
  

  uint16_t hues[NUM_CONES];
  uint8_t saturations[NUM_CONES]= {
    0,0,0,0,0,
    MAX_UINT8,MAX_UINT8,MAX_UINT8,MAX_UINT8,MAX_UINT8,
    MAX_UINT8,MAX_UINT8,MAX_UINT8,MAX_UINT8,MAX_UINT8,
    0,0,0,0,0
  };

  for (uint8_t ii{0}; ii<NUM_CONES; ++ii){
    hues[ii] = positive_mod(angles[ii]/(2*pi)*MAX_UINT16+color_offset,MAX_UINT16);
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





void doManualRotate(){
  // test_hsvrgb();

  setSymmetryModeFromButtons();
  
  switch (symmetry){
    
    case TwoFold:
      manual_rotate_twofold();
      break;

    case ThreeFold:
      manual_rotate_threefold();
      break;

    case FiveFold:
      manual_rotate_fivefold();
      break;

    case Reflect:
      manual_rotate_connected_cones();
      break;
  }
  
}
