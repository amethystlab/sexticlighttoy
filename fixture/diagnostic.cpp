#include "diagnostic.h"


void diagnostic_check(){
  // Store the current cone we are focusing on, as well as the 
  // previous state of the switch
  static int num = 1;
  static bool prevState = false;

  if(is_switch_on(0) != prevState){
    // if the switch is flipped from its previous position
    prevState = is_switch_on(0);
    clear();

    num = (num % MAX_CONE_NUM) + 1;
    //Iterate to the next cone number

#ifdef DEBUG_PRINT
    Serial.print("NUM CONE: ");
    Serial.println(num, DEC);
#endif

    // Light up the current cone (cone number num) green
    coneColor(num, 255, 0, 0, 0);
    for(int i = 0; i <= MAX_CONNECTION_NUM; i++){
      // Light up the cones connected to the current cone red
      uint8_t conn = get_connection(num, i);
#ifdef DEBUG_PRINT
      Serial.print("CONNECTED CONE: ");
      Serial.println(conn, DEC);
#endif
      coneColor(conn, 0, 255, 0, 0);
    }

    pixels.show();

    
  }
}



void diagnostic_check_threefold(){
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

    num = (num % MAX_CONE_NUM) + 1;
    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
    
    set_threefold(num);
    // set the axis cone in the cycles array
    set_threefold_cycles();
    // set the appropriate cycles in the cycles array

#ifdef DEBUG_PRINT
    Serial.print("Cycles: ");
    for(int i = 0; i < 20; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
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

void diagnostic_check_pentagons(){
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

    num = (num % MAX_CONE_NUM) + 1;
    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc
#ifdef DEBUG_PRINT
    Serial.print("NUM CONE: ");
    Serial.println(num, DEC);
#endif

    uint8_t next = get_connection(num, 0);
    set_pentagon(cycles, num, next, POSITIVE);
    // set the axis pentagon in the cycles array

    set_pentagon_cycles();
    // set the appropriate cycles in the cycles array
#ifdef DEBUG_PRINT
    Serial.print("PENTAGON: ");
    for(int i = 0; i < 5; i++){
      Serial.print(cycles[i], DEC);
      Serial.print(", ");
    }
    Serial.println("");
    Serial.print("Cycles: ");
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

void diagnostic_check_twofold(){
  num_per_rotation = 2;
  
  static uint8_t cone_num = 1;
  static uint8_t connection_num = 0;
  
  static bool prevState = false;
  static bool prevStateTwo = false;

  // Store the previous state of the switches
  // as well as the current axis cone

  if(is_switch_on(0) != prevState){
    // if we flip the first switch
    
    prevState = is_switch_on(0);
    clear();

    uint8_t secondCone = -1;
    bool found = false;

    connection_num++;
    if(connection_num > MAX_CONNECTION_NUM){
      connection_num = 0;
      cone_num = (cone_num % MAX_CONE_NUM) + 1;
    }
    
    while(!found){
      for(int i = connection_num; i <= MAX_CONNECTION_NUM && !found; i++){
        secondCone = get_connection(cone_num, i);
        if(secondCone > cone_num){
          found = true;
        }
      }

      if(!found){
        cone_num = (cone_num % MAX_CONE_NUM) + 1;
        connection_num = 0;
      }
    }

    // iterate the axis cone (so the center of rotation
    // becomes cone two if it was cone one before, etc

   
    set_twofold(cone_num, secondCone);
    // set the axis in the cycles array

    set_reflection_cycles();
    // set the appropriate cycles in the cycles array
    
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
    set_pentagon(arr, 1, 2, POSITIVE);
    
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
  diagnostic_check();
}
